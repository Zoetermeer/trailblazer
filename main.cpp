#include <string.h>
#include <stdio.h>
#include <math.h>
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
#include "lighting.hpp"
#include "GL.hpp"
#include "arm.hpp"
#include "exception.hpp"
#include "env.hpp"

#if defined(TEST)
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#endif

const int DEFAULT_WINSIZE = 1000;
const int UPDATE_EVERY_MS = 1000 / 60;

void init();
void drawBox(GLenum mode);
void draw();
void update(int arg);
void onWindowResize(int width, int height);
void onKeyPress(unsigned char key, int x, int y);
void onSpecialKeyPress(int key, int x, int y);
void onKeyUp(unsigned char key, int x, int y);
void onMouseClick(int buttonId, int upDown, int xw, int yw);
void onMouseMove(int xw, int yw);
void onPassiveMouseMove(int xw, int yw);

#define MOVE_DIST 1.0
#define MOVE_INTERVAL 0.5

enum class ViewportMode {
  PerspectiveOnly,
  QuadWithPerspective,
  QuadWithOrtho
};

typedef struct ui_state {
  vec2_t<int> mouse;
  vec2_t<int> button_down_mouse;
  vec2_t<int> window;
  ViewportMode viewport_mode;
} ui_state_t;

SolarSystem *ssys = NULL;
std::vector<Ship*> ships;
player_t the_player;
ui_state_t ui;
ShaderSet *shaders;
FirstPersonCamera fpsCam;

DefaultAmbientLighting lighting;

Arm right_arm(ArmType::Right, 5.0, 5.0, 2.0);
Arm left_arm(ArmType::Left, 5.0, 5.0, 2.0);

class ShipListener : public IShipLaunchListener, public IShipLandListener {
public:
  void onShipLaunch(Ship *ship)
  {
    ships.push_back(ship);
  }
  
  void onShipLand(Ship *ship)
  {
    std::vector<Ship*>::iterator it;
    for (it = ships.begin(); it != ships.end(); ++it) {
      if (*it == ship)
        break;
    }
    
    ships.erase(it);
  }
};

Ship *sh;

void displayInstructions()
{
  std::cout << "INSTRUCTIONS:" << std::endl;
  std::cout << "w - forward" << std::endl;
  std::cout << "s - backward" << std::endl;
  std::cout << "d - right" << std::endl;
  std::cout << "a - left" << std::endl;
}

void init()
{
  //Build shaders
  shaders = new ShaderSet();
  try {
    shaders->add(ShaderType::Default, "shaders/default.vert", "shaders/default.frag");
    shaders->add(ShaderType::Phong, "shaders/phong.vert", "shaders/phong.frag");
    shaders->add(ShaderType::Ship, "shaders/ship.vert", "shaders/ship.frag");
    shaders->add(ShaderType::Distortion, "shaders/distortion.vert", "shaders/phong.frag");
    shaders->add(ShaderType::Hemisphere, "shaders/hemisphere.vert", "shaders/default.frag");
    CHECK_OPENGL_ERROR;
  } catch (ShaderCompileException *e) {
    std::cout << e->what() << std::endl;
    delete e;
    exit(1);
    return;
  } catch (OpenGLException* oex) {
    std::cout << oex->what() << std::endl;
    delete oex;
    exit(1);
    return;
  } catch (FileNotFoundException *fx) {
    std::cout << fx->what() << std::endl;
    delete fx;
    exit(1);
    return;
  }
  
  glEnable(GL_DEPTH_TEST);
  ui.viewport_mode = ViewportMode::PerspectiveOnly;
  fpsCam.init(the_player);

  ShipListener *listener = new ShipListener();
  Events::addListener((IShipLaunchListener*)listener);
  Events::addListener((IShipLandListener*)listener);
  
  //Generate the solar system
  generator_params_t params = { 5000, 10 };
  ssys = SolarSystem::generate(params);
  std::cout << ssys->getName() << " system\n";
  ssys->animate();
  
  //Generate the arm geometry
  right_arm.generateGeometry();
  left_arm.generateGeometry();
  
  sh = new Ship(50.0);
}

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
  glutInit(&argc, argv);
  
  //It appears that although most (all?) macs running at least Lion
  //should have hardware supporting 3.2, but GLUT doesn't enable it by default.
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(DEFAULT_WINSIZE, DEFAULT_WINSIZE);
  ui.window.x = DEFAULT_WINSIZE;
  ui.window.y = DEFAULT_WINSIZE;

  glutCreateWindow("Space Explorer");
  
  memset(&the_player, 0, sizeof(player_t));
  the_player.offset.y = -2.0;
  the_player.offset.x = 50;
  Events::playerMoveEvent(the_player.offset, the_player.offset);
  
  glutDisplayFunc(draw);
  glutReshapeFunc(onWindowResize);
  glutKeyboardFunc(onKeyPress);
  glutSpecialFunc(onSpecialKeyPress);
  glutKeyboardUpFunc(onKeyUp);
  glutMouseFunc(onMouseClick);
  glutMotionFunc(onMouseMove);
  glutPassiveMotionFunc(onPassiveMouseMove);

  init();
  displayInstructions();

  update(0);
  glutMainLoop();
#endif
}

void onWindowResize(int width, int height)
{
  ui.window.x = width;
  ui.window.y = height;
}

int move_dir = DirectionIdle;

void update(int arg)
{
  ProcessList::advanceAll(UPDATE_EVERY_MS);
  glm::vec4 old_pos = the_player.offset;
  bool moved = false;
  
  //Move...
  if (move_dir & DirectionForward) {
    move(&the_player, MOVE_INTERVAL, DirectionForward);
    moved = true;
  }
  if (move_dir & DirectionBackward) {
    move(&the_player, MOVE_INTERVAL, DirectionBackward);
    moved = true;
  }
  if (move_dir & DirectionRight) {
    move(&the_player, MOVE_INTERVAL, DirectionRight);
    moved = true;
  }
  if (move_dir & DirectionLeft) {
    move(&the_player, MOVE_INTERVAL, DirectionLeft);
    moved = true;
  }
  if (move_dir == DirectionIdle) {
    right_arm.lower();
    left_arm.lower();
  }
  
  if (moved)
    Events::playerMoveEvent(old_pos, the_player.offset);

  glutPostRedisplay();
  glutTimerFunc(UPDATE_EVERY_MS, update, 0);
}

void onSpecialKeyPress(int key, int x, int y)
{
  switch (key)
  {
    case GLUT_KEY_UP:
      sh->position.y = sh->position.y.getValue() + 1.f;
      break;
    case GLUT_KEY_DOWN:
      sh->position.y = sh->position.y.getValue() - 1.f;
      break;
  }
}

void onKeyPress(unsigned char key, int x, int y)
{
  Process *proc = NULL;
  bool move = false;
  Direction dir;
  switch (key)
  {
    case 'w': /* Up */
      move_dir |= DirectionForward;
      right_arm.swimForward();
      left_arm.swimForward();
      break;
    case 's': /* Down */
      move_dir |= DirectionBackward;
      break;
    case 'a': /* Left */
      move_dir |= DirectionLeft;
      right_arm.swimForward();
      break;
    case 'd': /* Right */
      move_dir |= DirectionRight;
      left_arm.swimForward();
      break;
    case 'q':
      the_player.yaw = nextAngle(the_player.yaw, -5.0);
      break;
    case 'e':
      the_player.yaw = nextAngle(the_player.yaw, 5.0);
      break;
    case ' ': /* Jump */
      ui.viewport_mode = ui.viewport_mode == ViewportMode::PerspectiveOnly ?
                                              ViewportMode::QuadWithPerspective :
                                              ViewportMode::PerspectiveOnly;
      break;
    case 'o':
      if (ui.viewport_mode == ViewportMode::PerspectiveOnly)
        break;
      ui.viewport_mode = ui.viewport_mode == ViewportMode::QuadWithPerspective ?
      ViewportMode::QuadWithOrtho : ViewportMode::QuadWithPerspective;
      break;
  }
  
  if (proc)
    ProcessList::add(proc);
}

void onKeyUp(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 'w':
      move_dir ^= DirectionForward;
      break;
    case 's':
      move_dir ^= DirectionBackward;
      break;
    case 'a':
      move_dir ^= DirectionLeft;
      break;
    case 'd':
      move_dir ^= DirectionRight;
      break;
  }
}

void onMouseClick(int buttonId, int upDown, int xw, int yw)
{
  
}

void onMouseMove(int xw, int yw)
{
  int xdelt = xw - ui.button_down_mouse.x;
  the_player.roll = nextAngle(the_player.roll, xdelt);
  
  ui.button_down_mouse.x = xw;
  ui.button_down_mouse.y = yw;
  
  /* Update regular x/y coords for the mouse, so regular 
     move events don't have skewed deltas */
  ui.mouse.x = xw;
  ui.mouse.y = yw;
  
  Events::playerLookEvent({ the_player.roll, the_player.pitch, the_player.yaw });
}

void onPassiveMouseMove(int xw, int yw)
{
  int xdelt = xw - ui.mouse.x, ydelt = yw - ui.mouse.y;
  if (xw >= ui.window.x || yw >= ui.window.y) {
    /* Mark, but don't handle */
    ui.mouse.x = xw;
    ui.mouse.y = yw;
    return;
  }
  
  GLfloat new_pitch = nextAngle(the_player.pitch, ydelt);
  /* Bounds on the pitch of the player's head */
  if (new_pitch <= 70.0 || new_pitch >= 290.0)
    the_player.pitch = new_pitch;
  
  the_player.yaw = nextAngle(the_player.yaw, xdelt);
  ui.mouse.x = xw;
  ui.mouse.y = yw;
  
  /* Update the 'mouse down' coordinates as well, 
     so the deltas for the first mouse down+drag event 
     aren't wildly skewed */
  ui.button_down_mouse.x = xw;
  ui.button_down_mouse.y = yw;
  
  Events::playerLookEvent({ the_player.roll, the_player.pitch, the_player.yaw });
}

void drawScene(ICamera &camera, bool drawArms)
{
  try {
    MatrixStack mv, proj;
    Env env(mv, proj, *shaders);
    camera.draw(ui.window.x, ui.window.y, proj);
    CHECK_OPENGL_ERROR;
    
    //Initialize the GL stack with the identity matrix
    //(in case any drawing code is still not using shaders)
    mv.syncToGlStack(GL_MODELVIEW);

    shaders->prepareDefault(env, GL::GRAY);
    GL::drawPlane();
    shaders->current().uniform(Uniform::COLOR) = GL::RED;
    GL::drawAxes(env);

    ssys->draw(env);
    if (drawArms) {
      right_arm.draw(env);
      left_arm.draw(env);
    }
    
    //Draw ships
    
    //sh.position.x = 20.f;
    //sh.position.y = -20.f;
    //sh.position.z = 20.f;
    //shaders->prepareShip(env, glm::vec4(0.2, 0.2, 0.2, 1.0), GL::GRAY, GL::WHITE);
    //sh->draw(env);
    
    for (Ship *ship : ships) {
      ship->draw(env);
    }
  } catch (OpenGLException *ex) {
    std::cout << "OpenGL Exception: " << ex->what() << std::endl;
    delete ex;
    exit(1);
  }
}

void draw()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, ui.window.x, ui.window.y);
  drawScene(fpsCam, true);
  
  glutSwapBuffers();
}
