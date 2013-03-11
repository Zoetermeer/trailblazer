#include <string.h>
#include <iostream>
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
#include "chunk.hpp"
#include "sky.hpp"

#if defined(TEST)
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#endif

const int DEFAULT_WIDTH = 1600;
const int DEFAULT_HEIGHT = 1000;

class WorldSandboxApp : public OpenGLApp {
private:
  Player m_player;
  Direction m_moveDir;
  FirstPersonCamera m_camera;
  Sky *m_sky;
  
  Arm m_rightArm;
  Arm m_leftArm;
  
  ChunkBuffer m_cbuffer;
  GLfloat m_heightMap[32][32];
  const int NUM_VOXELS = 32;
  
public:
  WorldSandboxApp()
  : m_rightArm(ArmType::Right, 5.0, 5.0, 2.0),
    m_leftArm(ArmType::Left, 5.0, 5.0, 2.0),
    m_moveDir(Direction::Idle)
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
      shaders.add(new HemisphereAOShaderProgram());
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
    std::cout << "w - forward" << std::endl;
    std::cout << "s - backward" << std::endl;
    std::cout << "d - right" << std::endl;
    std::cout << "a - left" << std::endl;
    std::cout << "spacebar - fly up" << std::endl;
    std::cout << "shift - fly down" << std::endl;
  }
  
protected:
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
    m_rightArm.generateGeometry();
    m_leftArm.generateGeometry();
    
    displayInstructions();
    
    //Generate the initial state of the world
    m_cbuffer.init();
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
      GL::drawPlane();
      shaders.current().uniform(Uniform::COLOR) = GL::RED;
      GL::drawAxes(env);
      
      Sky::getInstance().draw(env);
      
      //Draw the terrain
      m_cbuffer.draw(env);
      
      //Draw a sphere at the origin to test Phong
      auto playerPos = glm::vec3(m_player.getOffset());
      mv.translateY(200.f);
      shaders.preparePhong(env,
                           glm::vec3(Sky::getSunPosition()),
                           playerPos,
                           m_player.getIsHeadlightOn(), 
                           GL::color(178, 34, 34),
                           GL::color(205, 92, 92),
                           GL::WHITE);
      glutSolidSphere(100, 20, 20);
      
      m_rightArm.draw(env);
      m_leftArm.draw(env);
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
#if defined(TEST)
  
  printf("RUNNING TESTS\n");
  CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(suite);
  
  runner.setOutputter(new CppUnit::CompilerOutputter(&runner.result(), std::cerr));
  bool success = runner.run();

  return success ? 0 : 1;
  
#else 
  
  WorldSandboxApp app;
  app.setConfineMouseCursor(true);
  app.run(DEFAULT_WIDTH, DEFAULT_HEIGHT, "World Sandbox");

#endif
}
