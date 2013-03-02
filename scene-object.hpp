#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include <glm/glm.hpp>

class Env;

class SceneObject {
protected:
  unsigned int m_id;
  glm::mat4 m_world;
  
public:
  SceneObject()
  {
    static unsigned int id = 0;
    m_id = ++id;
    m_world = glm::mat4(1.0);
  }
  
public:
  glm::mat4 getWorld() const { return m_world; }
  
  virtual glm::vec4 toWorld()
  {
    glm::vec4 v(0.f, 0.f, 0.f, 1.f);
    return m_world * v;
  }
  
  virtual void draw(Env &env) = 0;
};

#endif