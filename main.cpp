#include <string.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <GL/glfw.h>
#include "common.hpp"
#include "matrix-stack.hpp"
#include "player.hpp"
#include "process.hpp"
#include "solar-system.hpp"
#include "cameras.hpp"
#include "shader.hpp"
#include "events.hpp"
#include "lighting.hpp"
#include "GL.hpp"
#include "arm.hpp"
#include "exception.hpp"
#include "env.hpp"
#include "opengl-app.hpp"

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
  }
  
  virtual void onMouseMove(const glm::ivec2 &oldPos, const glm::ivec2 &newPos)
  {
    m_player.mouseLook(oldPos.x, oldPos.y, newPos.x, newPos.y);
   
    //TODO: Implement when left-button pressed
    //the_player.mouseRoll(oldX, oldY, xw, yw);
  }
  
  virtual void onKeyDown(char key)
  {
    bool move = false;
    Direction dir;
    switch (key)
    {
      case 'w': /* Up */
        m_moveDir = m_moveDir | Direction::Forward;
        m_rightArm.swimForward();
        m_leftArm.swimForward();
        break;
      case 's': /* Down */
        m_moveDir = m_moveDir | Direction::Backward;
        break;
      case 'a': /* Left */
        m_moveDir = m_moveDir | Direction::Left;
        m_rightArm.swimForward();
        break;
      case 'd': /* Right */
        m_moveDir = m_moveDir | Direction::Right;
        m_leftArm.swimForward();
        break;
      case 'q':
        m_player.getAttitude().yaw = nextAngle(m_player.getAttitude().yaw, -5.0);
        break;
      case 'e':
        m_player.getAttitude().yaw = nextAngle(m_player.getAttitude().yaw, 5.0);
        break;
    }
  }
  
  virtual void onKeyUp(char key)
  {
    switch (key)
    {
      case 'w':
        m_moveDir = m_moveDir ^ Direction::Forward;
        break;
      case 's':
        m_moveDir = m_moveDir ^ Direction::Backward;
        break;
      case 'a':
        m_moveDir = m_moveDir ^ Direction::Left;
        break;
      case 'd':
        m_moveDir = m_moveDir ^ Direction::Right;
        break;
    }
  }
  
  virtual void update(int deltaMs)
  {
    ProcessList::advanceAll(deltaMs);
    glm::vec4 old_pos = m_player.getOffset();
    bool moved = false;
    const int MOVE_INTERVAL = 1.f;
    
    //Move...
    if ((m_moveDir & Direction::Forward) > Direction::Idle) {
      m_player.move(MOVE_INTERVAL, Direction::Forward);
      moved = true;
    }
    if ((m_moveDir & Direction::Backward) > Direction::Idle) {
      m_player.move(MOVE_INTERVAL, Direction::Backward);
      moved = true;
    }
    if ((m_moveDir & Direction::Right) > Direction::Idle) {
      m_player.move(MOVE_INTERVAL, Direction::Right);
      moved = true;
    }
    if ((m_moveDir & Direction::Left) > Direction::Idle) {
      m_player.move(MOVE_INTERVAL, Direction::Left);
      moved = true;
    }
    if (m_moveDir == Direction::Idle) {
      m_rightArm.lower();
      m_leftArm.lower();
    }
    
    if (moved)
      Events::playerMoveEvent(old_pos, m_player.getOffset());
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
      
      m_ssys->draw(env);
      m_rightArm.draw(env);
      m_leftArm.draw(env);
      
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
