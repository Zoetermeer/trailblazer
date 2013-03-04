#ifndef OPENGL_APP_H
#define OPENGL_APP_H

#include "common.hpp"
#include "env.hpp"
#include "shader.hpp"

class OpenGLApp {
private:
  bool m_running;
  int m_msBetweenUpdate;
  int m_msBetweenDraw;
  bool m_confineCursor;
  glm::ivec2 m_windowSize;
  glm::ivec2 m_mousePos;
  ShaderSet m_shaders;
  
  //GLFW wants callback functions for certain events,
  //so we need a way to handle these events statically but
  //communicate them to an OpenGLApp instance.  Because an app
  //is essentially a singleton, we stash the current one here.
  static OpenGLApp *s_appInstance;
  
public:
  OpenGLApp()
  : m_running(false), m_msBetweenUpdate(DEFAULT_UPDATE_INTERVAL),
    m_msBetweenDraw(0), m_confineCursor(false),
    m_mousePos(0, 0)
  {
    s_appInstance = this;
  }
  
  ~OpenGLApp()
  {
    s_appInstance = NULL;
  }
  
public:
  void setConfineMouseCursor(bool v);
  glm::ivec2 getMousePos() const { return m_mousePos; }
  glm::ivec2 getWindowSize() const { return m_windowSize; }
  
private:
  void queryInput();
  void queryWindowState();
  static void handleSpecialKeyEvent(int key, int state);
  static void handleKeyEvent(int key, int state);
  static void handleMouseButtonEvent(int button, int state);
  
protected:
  virtual void update(int deltaMs) = 0;
  virtual void draw(Env &env) = 0;
  virtual void init(Env &env);
  void swapBuffers();
  
  virtual void onResize(int width, int height);
  virtual void onMouseMove(const glm::ivec2 &oldPos, const glm::ivec2 &newPos);
  virtual void onMouseDown(int button);
  virtual void onMouseUp(int button);
  virtual void onKeyDown(char key);
  virtual void onKeyUp(char key);
  void quit();
  
public:
  void run(int winSizeX, int winSizeY, const char *windowTitle);
};

#endif