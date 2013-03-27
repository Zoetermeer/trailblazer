#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>
#include "common.hpp"
#include "matrix-stack.hpp"
#include "player.hpp"
#include "process.hpp"
#include "cameras.hpp"
#include "shader.hpp"
#include "events.hpp"
#include "GL.hpp"
#include "arm.hpp"
#include "exception.hpp"
#include "env.hpp"
#include "opengl-app.hpp"
#include "chunk-buffer.hpp"
#include "cube-chunk.hpp"
#include "poly-chunk.hpp"
#include "sky.hpp"
#include "units.hpp"

#if defined(RUN_TESTS)
#include "tests/chunk-tests.hpp"
#include "tests/units-tests.hpp"
#endif

const int DEFAULT_WIDTH = 1600;
const int DEFAULT_HEIGHT = 1000;

class WorldSandboxApp : public OpenGLApp {
private:
  Player m_player;
  Direction m_moveDir;
  FirstPersonCamera m_camera;
  Sky *m_sky;
  
  bool m_hideArms;
  
  ChunkBuffer<PolyChunk> m_cbuffer;
  
public:
  WorldSandboxApp()
  : m_moveDir(Direction::Idle), m_hideArms(true)
  {
    
  }
  
  ~WorldSandboxApp()
  {
    
  }
  
private:
  void initShaders(Env &env)
  {
    ShaderSet &shaders = env.getShaders();
    try {
      shaders.add(ShaderType::Default, "shaders/default.vert", "shaders/default.frag");
      shaders.add(ShaderType::Phong, "shaders/phong.vert", "shaders/phong.frag");
      shaders.add(ShaderType::Hemisphere, "shaders/hemisphere.vert", "shaders/default.frag");
      shaders.add(new VoxelAOShaderProgram());
      CHECK_OPENGL_ERROR;
    } catch (std::exception *ex) {
      std::cout << ex->what() << std::endl;
      delete ex;
      exit(1);
      return;
    }
  }
  
  void displayInstructions()
  {
    std::cout << "INSTRUCTIONS:" << std::endl;
    std::cout << "mouse - look around" << std::endl;
    std::cout << "w: forward" << std::endl;
    std::cout << "s: backward" << std::endl;
    std::cout << "d: right" << std::endl;
    std::cout << "a: left" << std::endl;
    std::cout << "<up>: speed up" << std::endl;
    std::cout << "<down>: slow down" << std::endl;
    std::cout << "spacebar: fly up" << std::endl;
    std::cout << "shift: fly down" << std::endl;
    std::cout << "h: hide/show arms" << std::endl;
    std::cout << "<period>: headlight on/off" << std::endl;
    std::cout << "enter: - blow it up" << std::endl;
  }
  
protected:
  PolyChunk *m_testChunk;
  PolyChunk *m_testChunk2;
  virtual void init(Env &env)
  {
    OpenGLApp::init(env);
    this->initShaders(env);
    
    glEnable(GL_DEPTH_TEST);
    
    //Set up the player
    glm::vec4 startPos(0.f, 100.f, 0.f, 1.f);
    m_player.setOffset(startPos);
    Events::playerMoveEvent(startPos, startPos);
    
    //Camera
    m_camera.init(m_player);
    
    Sky::init();
    
    //Generate the arm geometry
    //m_rightArm.generateGeometry();
    //m_leftArm.generateGeometry();
    
    displayInstructions();
    
    //Generate the initial state of the world
    m_cbuffer.init();
//    m_testChunk = new PolyChunk(0, 0, 0);
//    m_testChunk->generateAsync();
//    m_testChunk2 = new PolyChunk(1, 0, 0);
//    m_testChunk2->generateAsync();
  }
  
  virtual void onMouseMove(const glm::ivec2 &oldPos, const glm::ivec2 &newPos)
  {
    m_player.mouseLook(oldPos.x, oldPos.y, newPos.x, newPos.y);
   
    //TODO: Implement when left-button pressed
    //the_player.mouseRoll(oldX, oldY, xw, yw);
    
    //Calculate the "picking ray", always from the center
    //of the viewing window
    glm::ivec2 ws = getWindowSize();
    GLfloat fw = (GLfloat)ws.x, fh = (GLfloat)ws.y;
    GLfloat x = fw / 2.f, y = fh / 2.f;
    
    glm::vec3 view = m_camera.getLookVector() - glm::vec3(m_camera.getPos());
    view = glm::normalize(view);
    
    glm::vec3 h = glm::cross(view, m_camera.getUpVector());
    h = glm::normalize(h);
    
    glm::vec3 v = glm::cross(h, view);
    v = glm::normalize(v);
    
    GLfloat rad = DEG_TO_RAD(m_camera.getFOV());
    GLfloat vlen = tan(rad / 2) * m_camera.getNearClippingPlaneDist();
    GLfloat hlen = vlen * (fw / fh);
    
    v = v * vlen;
    h = h * hlen;
    
    x -= fw / 2;
    y -= fh / 2;
    y /= (fh / 2);
    x /= (fw / 2);
    
    glm::vec3 pos = glm::vec3(m_camera.getPos()) + view * m_camera.getNearClippingPlaneDist() + h * x + v * y;
    glm::vec3 dir = pos - glm::vec3(m_camera.getPos());
    
    //Test for intersection here?
    
  }
  
  virtual void onKeyDown(int key)
  {
    switch (key)
    {
        case 'h':
        case 'H':
          m_hideArms = !m_hideArms;
          break;
    }
    Events::keyDownEvent(key, false);
  }
  
  virtual void onKeyUp(int key)
  {
    Events::keyUpEvent(key, false);
  }
  
  virtual void update(int deltaMs)
  {
    ProcessList::advanceAll(deltaMs);
  }
  
  virtual void draw(Env &env)
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::ivec2 winSz = this->getWindowSize();
    glViewport(0, 0, winSz.x, winSz.y);
    
    //Environment vars
    env.setPlayer(&m_player);
    env.setSunPos(Sky::getSunPosition());
    try {
      MatrixStack &mv = env.getMV();
      ShaderSet &shaders = env.getShaders();
      MatrixStack &proj = env.getProj();
      
      mv.loadIdentity();
      proj.loadIdentity();
      
      m_camera.draw(winSz.x, winSz.y, proj);
      CHECK_OPENGL_ERROR;
      
      //Initialize the GL stack with the identity matrix
      //(in case any drawing code is still not using shaders)
      mv.syncToGlStack(GL_MODELVIEW);
      
      shaders.prepareDefault(env, GL::GRAY);
      //GL::drawPlane();
      shaders.current().uniform(Uniform::COLOR) = GL::RED;
      GL::drawAxes(env);
      
      Sky::getInstance().draw(env);
      
      //Draw the terrain
      m_cbuffer.draw(env);
//      if (m_testChunk->generateAsync())
//        m_testChunk->draw(env, glm::vec4(0,0,0,0), glm::vec3(0,0,0), false, false, 0.f);
//      
//      if (m_testChunk2->generateAsync())
//        m_testChunk2->draw(env, glm::vec4(0, 0, 0, 0), glm::vec3(0, 0, 0), false, false, 0.f);
      
      //Draw status text
      mv.pushMatrix();
      proj.pushMatrix();
      {
        proj.loadIdentity();
        proj.ortho2d(0, winSz.x, 0, winSz.y, -1, 1);
        
        std::stringstream str;
        debug_stats &stats = env.getStats();
        double pctActive = ((double)stats.active_voxels / (double)stats.voxels) * 100.0;
        double pctDrawn = ((double)stats.drawn_voxels / (double)stats.voxels) * 100.0;
        double vsPerVoxel = ((double)stats.vertices / (double)stats.drawn_voxels);
        str << "Vertices: " << stats.vertices;
        str << " Voxels: " << stats.voxels;
        str << " Active voxels: " << stats.active_voxels << " (" << pctActive << "%)";
        str << " Drawn voxels: " << stats.drawn_voxels << " (" << pctDrawn << "%)";
        str << " Avg. verts per voxel: " << vsPerVoxel;
        shaders.prepareDefault(env, GL::WHITE);
        GL::drawText(10.f, 20.f, -.5f, str.str().c_str());
        
        std::stringstream str2;
        str2 << "Load queue size: " << stats.load_queue_size;
        str2 << " Render queue size: " << stats.render_queue_size;
        GL::drawText(10.f, 40.f, -.5f, str2.str().c_str());
        
        std::stringstream fpsStr;
        fpsStr << "Frames per second: " << stats.fps;
        GL::drawText(10.f, 60.f, -.5f, fpsStr.str().c_str());
        
        std::stringstream posStr;
        std::stringstream vStr;
        std::stringstream lStr;
        glm::vec4 ppos = m_player.getOffset();
        glm::ivec3 vc = Units::worldToVoxel(ppos);
        glm::vec2 ll = Units::worldToLatLon(ppos);
        posStr << "World: (" << ppos.x << ", " << ppos.y << ", " << ppos.z << ")  ";
        vStr << "Voxel: (" << vc.x << ", " << vc.y << ", " << vc.z << ")";
        lStr << "Lat/lon: " << ll.x << "°, " << ll.y << "°";
        GL::drawText(10.f, 80.f, -.5f, posStr.str().c_str());
        GL::drawText(10.f, 100.f, -.5f, vStr.str().c_str());
        GL::drawText(10.f, 120.f, -.5f, lStr.str().c_str());
      }
      proj.popMatrix();
      mv.popMatrix();
    } catch (OpenGLException *ex) {
      std::cout << "OpenGL Exception: " << ex->what() << std::endl;
      delete ex;
      exit(1);
    }
    
    this->swapBuffers();
  }
};

int main(int argc, char **argv)
{
#if defined(RUN_TESTS)
  ::testing::InitGoogleTest(&argc, argv);
  if (RUN_ALL_TESTS()) {
    return 1;
  }
#endif
  
  WorldSandboxApp app;
  app.setConfineMouseCursor(true);
  app.run(DEFAULT_WIDTH, DEFAULT_HEIGHT, "World Sandbox");
}
