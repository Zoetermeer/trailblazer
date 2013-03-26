#ifndef CAMERAS_H
#define CAMERAS_H

#include "common.hpp"
#include "events.hpp"
#include "player.hpp"

class MatrixStack;

class ICamera {
public:
  virtual void draw(int winWidth, int winHeight, MatrixStack &proj) = 0;
};

class IsometricCamera : public ICamera, public IPlayerMoveListener, public IPlayerLookListener {
private:
  glm::vec4 m_pos;
  vec_t<GLfloat, 4> m_camArgs;
  Attitude m_orientation;
  
public:
  IsometricCamera(GLfloat pitch, GLfloat roll, GLfloat yaw)
  {
    m_orientation.pitch = pitch;
    m_orientation.roll = roll;
    m_orientation.yaw = yaw;
    Events::addListener((IPlayerMoveListener*)this);
    Events::addListener((IPlayerLookListener*)this);
  }
  
  ~IsometricCamera()
  {
    Events::removeListener(EventPlayerMove, this);
    Events::removeListener(EventPlayerLook, this);
  }
  
public:
  void init(const Player &player);
  void draw(int winWidth, int winHeight, MatrixStack &proj);
  void onPlayerMove(const glm::vec4 &oldPos, const glm::vec4 &newPos);
  void onPlayerLook(const Attitude &att, const glm::vec3 &lookVector, bool headlightOn);
};

class FirstPersonCamera : public ICamera, public IPlayerMoveListener, public IPlayerLookListener {
private:
  Attitude m_attitude;
  glm::vec4 m_pos;
  glm::vec3 m_lookVector;
  glm::vec3 m_upVector;

public:
  FirstPersonCamera()
  {
    Events::addListener((IPlayerMoveListener*)this);
    Events::addListener((IPlayerLookListener*)this);
  }

  ~FirstPersonCamera()
  {
    Events::removeListener(EventPlayerMove, this);
    Events::removeListener(EventPlayerLook, this);
  }

public:
  Attitude getAttitude() const { return m_attitude; }
  glm::vec4 getPos() const { return m_pos; }
  glm::vec3 getLookVector() const { return m_lookVector; }
  glm::vec3 getUpVector() const { return m_upVector; }
  GLfloat getFOV() const { return 45.f; }
  GLfloat getNearClippingPlaneDist() const { return 1.f; }
  GLfloat getFarClippingPlaneDist() const { return 50000.f; }
  void setPos(glm::vec4 &v) { m_pos = v; }
  void setPos(GLfloat x, GLfloat y, GLfloat z)
  {
    m_pos.x = x;
    m_pos.y = y;
    m_pos.z = z;
  }

public:
  void init(const Player &player);
  void draw(int winWidth, int winHeight, MatrixStack &proj);
  void onPlayerMove(const glm::vec4 &old_pos, const glm::vec4 &new_pos);
  void onPlayerLook(const Attitude &att, const glm::vec3 &lookVector, bool headlightOn);
};

#endif
