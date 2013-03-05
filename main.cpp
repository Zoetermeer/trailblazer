#include <string.h>
#include <iostream>
#include <vector>
#include "common.hpp"
#include "matrix-stack.hpp"
#include "player.hpp"
#include "process.hpp"
#include "solar-system.hpp"
#include "cameras.hpp"
#include "shader.hpp"
#include "events.hpp"
#include "GL.hpp"
#include "arm.hpp"
#include "exception.hpp"
#include "env.hpp"
#include "opengl-app.hpp"
#include "chunk.hpp"
#include <noise.h>

#if defined(TEST)
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#endif

const int DEFAULT_WIDTH = 1600;
const int DEFAULT_HEIGHT = 1000;

class WorldSandboxApp : public OpenGLApp {
private:
  SolarSystem *m_ssys = NULL;
  std::vector<Ship*> m_ships;
  Player m_player;
  Direction m_moveDir;
  FirstPersonCamera m_camera;
  
  Arm m_rightArm;
  Arm m_leftArm;
  
  Chunk *m_chunk1;
  Chunk *m_chunk2;
  Chunk *m_chunk3;
  Chunk *m_chunk4;
  GLfloat m_heightMap[32][32];
  const int NUM_VOXELS = 32;
  
public:
  WorldSandboxApp()
  : m_rightArm(ArmType::Right, 5.0, 5.0, 2.0),
    m_leftArm(ArmType::Left, 5.0, 5.0, 2.0),
    m_moveDir(Direction::Idle)
  {
    
  }
  
private:
  void initShaders(Env &env)
  {
    ShaderSet &shaders = env.getShaders();
    try {
      shaders.add(ShaderType::Default, "shaders/default.vert", "shaders/default.frag");
      shaders.add(ShaderType::Phong, "shaders/phong.vert", "shaders/phong.frag");
      shaders.add(ShaderType::Ship, "shaders/ship.vert", "shaders/ship.frag");
      shaders.add(ShaderType::Distortion, "shaders/distortion.vert", "shaders/phong.frag");
      shaders.add(ShaderType::Hemisphere, "shaders/hemisphere.vert", "shaders/default.frag");
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
    std::cout << "w - forward" << std::endl;
    std::cout << "s - backward" << std::endl;
    std::cout << "d - right" << std::endl;
    std::cout << "a - left" << std::endl;
  }
  
protected:
  virtual void init(Env &env)
  {
    OpenGLApp::init(env);
    this->initShaders(env);
    
    glEnable(GL_DEPTH_TEST);
    
    //Set up the player
    glm::vec4 startPos(0.f, -2.f, 50.f, 1.f);
    m_player.setOffset(startPos);
    Events::playerMoveEvent(startPos, startPos);
    
    //Camera
    m_camera.init(m_player);
    
    //Generate the solar system
    generator_params_t params = { 5000, 10 };
    m_ssys = SolarSystem::generate(params);
    std::cout << m_ssys->getName() << " system\n";
    m_ssys->animate();
    
    //Generate the arm geometry
    m_rightArm.generateGeometry();
    m_leftArm.generateGeometry();
    
    displayInstructions();
    
    //Generate the height map
    m_chunk1 = new Chunk(0, 0);
    m_chunk2 = new Chunk(1, 0);
    m_chunk3 = new Chunk(0, 1);
    m_chunk4 = new Chunk(1, 1);
    m_chunk1->generate();
    m_chunk2->generate();
    m_chunk3->generate();
    m_chunk4->generate();
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
      
      //Draw a yellow sphere at player's position for testing
      mv.pushMatrix();
      {
        glm::vec4 poff = m_player.getOffset();
        mv.translate(poff.x, poff.y, poff.z);
        Attitude &att = m_player.getAttitude();
        mv.rotateY(att.yaw);
        mv.rotateZ(att.roll);
        mv.rotateX(att.pitch);
        mv.translate(0.f, 3.f, -20.f);
        
        mv.scaleZ(.1f);
        shaders.prepareDefault(env, GL::YELLOW);
        glutSolidSphere(1.f, 20, 20);
      }
      mv.popMatrix();
      
      //Draw the terrain
      m_chunk1->draw(env);
      m_chunk2->draw(env);
      m_chunk3->draw(env);
      m_chunk4->draw(env);
      
      //Draw a solid ground plane
      /*
      mv.pushMatrix();
      {
        mv.translateY(-1.f);
        mv.scale(2000.f, 1.f, 2000.f);
        shaders.prepareHemisphere(env, glm::vec3(0.f, 100.f, 0.f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec4(0.0f, 0.0f, 0.3f, 1.f));
        GL::drawBox(GL_TRIANGLES);
      }
      mv.popMatrix();
       */
      
      //m_ssys->draw(env);
      //m_rightArm.draw(env);
      //m_leftArm.draw(env);
      
      //Draw ships
      for (Ship *ship : m_ships) {
        ship->draw(env);
      }
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
