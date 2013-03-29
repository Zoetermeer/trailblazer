#include "opengl-app.hpp"
#include <GL/glfw.h>

OpenGLApp *OpenGLApp::s_appInstance = NULL;

void OpenGLApp::setConfineMouseCursor(bool v)
{
  m_confineCursor = v;
}

void OpenGLApp::queryInput()
{
  int mx, my;
  glfwGetMousePos(&mx, &my);
  if (mx != m_mousePos.x || my != m_mousePos.y) {
    onMouseMove(m_mousePos, glm::ivec2(mx, my));
    m_mousePos.x = mx;
    m_mousePos.y = my;
  }
  

}

void OpenGLApp::queryWindowState()
{
  int w, h;
  glfwGetWindowSize(&w, &h);
  if (w != m_windowSize.x || h != m_windowSize.y)
    onResize(w, h);
}

void OpenGLApp::handleKeyEvent(int key, int state)
{
  if (!s_appInstance)
    return;
  
  if (state == GLFW_PRESS)
    s_appInstance->onKeyDown(key);
  else
    s_appInstance->onKeyUp(key);
}

void OpenGLApp::handleMouseButtonEvent(int button, int state)
{
  if (!s_appInstance)
    return;
  
  if (state == GLFW_PRESS)
    s_appInstance->onMouseDown(button);
  else
    s_appInstance->onMouseUp(button);
}

void OpenGLApp::init(Env &env)
{
  
}

void OpenGLApp::swapBuffers()
{
  glfwSwapBuffers();
}

void OpenGLApp::run(int winSizeX, int winSizeY, const char *windowTitle)
{
  glfwInit();
  m_windowSize.x = winSizeX;
  m_windowSize.y = winSizeY;
  glfwOpenWindow(winSizeX, winSizeY, 8, 8, 8, 8, 24, 0, GLFW_WINDOW);
  glfwSetWindowTitle(windowTitle);
  if (m_confineCursor)
    glfwDisable(GLFW_MOUSE_CURSOR);
  else
    glfwEnable(GLFW_MOUSE_CURSOR);
  
  glfwSetKeyCallback(OpenGLApp::handleKeyEvent);
  glfwSetMouseButtonCallback(OpenGLApp::handleMouseButtonEvent);
  
  //Must be done after opening the window (otherwise no GL context)
  Env e(m_shaders, m_textures);
  init(e);
  
  double t, updateLast = 0.0, drawLast = 0.0;
  int udelt, ddelt;
  m_running = true;  
  while (m_running) {
    t = glfwGetTime();
    udelt = (int)((t - updateLast) * 1000.0);
    ddelt = (int)((t - drawLast) * 1000.0);
    if (udelt >= m_msBetweenUpdate) {
      updateLast = t;
      queryInput();
      update(udelt);
    }
    
    if (ddelt >= m_msBetweenDraw) {
      double fps = 1.0 / (t - drawLast);
      drawLast = t;
      Env env(m_shaders, m_textures);
      env.getStats().fps = fps;
      draw(env);
    }
  }
  
  glfwTerminate();
}

void OpenGLApp::quit()
{
  m_running = false;
}

void OpenGLApp::onResize(int width, int height) { }
void OpenGLApp::onMouseMove(const glm::ivec2 &oldPos, const glm::ivec2 &newPos) { }
void OpenGLApp::onMouseDown(int button) { }
void OpenGLApp::onMouseUp(int button) { }
void OpenGLApp::onKeyDown(int key) { }
void OpenGLApp::onKeyUp(int key) { }