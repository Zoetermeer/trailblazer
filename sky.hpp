#ifndef SKY_H
#define SKY_H

#include "scene-object.hpp"
#include "process.hpp"
#include "events.hpp"
#include "env.hpp"

class Sky : public SceneObject, public Process, public IPlayerMoveListener {
private:
  GLfloat m_sunZRotation;
  glm::vec4 m_playerPos;
  
  static Sky *s_instance;
  
private:
  Sky() : m_sunZRotation(90.f)
  {
    ProcessList::add(this);
    Events::addListener((IPlayerMoveListener*)this);
  }
  
public:
  ~Sky()
  {
    ProcessList::remove(this);
    Events::removeListener(EventPlayerMove, this);
  }
  
public:
  static void init();
  static glm::vec4 getSunPosition();
  static Sky &getInstance();
  
public:
  virtual void draw(Env &env);
  virtual void advance(int deltaMs);
  virtual bool isDone();
  void onPlayerMove(const glm::vec4 &old_pos, const glm::vec4 &new_pos);
};

#endif