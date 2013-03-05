#include "cameras.hpp"
#include "GL.hpp"
#include "matrix-stack.hpp"

#define HEAD_Y 3.0

void IsometricCamera::init(const Player &player)
{
  m_pos = player.getOffset();
}

void IsometricCamera::draw(int winWidth, int winHeight, MatrixStack &proj)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(m_pos.x - 50.0,
          m_pos.x + 50.0,
          m_pos.y  + 50.0,
          m_pos.y - 50.0,
          1.0,
          1000.0);
  glRotatef(m_orientation.yaw, 0.0, 1.0, 0.0);
  glRotatef(m_orientation.pitch, 1.0, 0.0, 0.0);
  glTranslatef(m_pos.x + 1.0, m_pos.y, m_pos.z + 1.0);
}

void IsometricCamera::onPlayerMove(const glm::vec4 &oldPos, const glm::vec4 &newPos)
{
  m_pos = newPos;
}

void IsometricCamera::onPlayerLook(const Attitude &att)
{
  
}

void FirstPersonCamera::init(const Player &player)
{
  m_pos.x = player.getOffset().x;
  m_pos.y = player.getOffset().y - HEAD_Y;
  m_pos.z = player.getOffset().z;
}
#include <iostream>

void FirstPersonCamera::draw(int winWidth, int winHeight, MatrixStack &proj)
{
  proj.loadIdentity();
  proj.perspective(getFOV(),
                   (GLfloat)winWidth / (GLfloat)winHeight,
                   getNearClippingPlaneDist(),
                   getFarClippingPlaneDist());
  proj.rotateZ(m_attitude.roll);
  proj.rotateX(-m_attitude.pitch);
  proj.rotateY(-m_attitude.yaw);
  proj.translate(-m_pos.x, -m_pos.y, -m_pos.z);
  
  m_lookVector = glm::vec3(proj.getCurrent() * glm::vec4(0.f, 0.f, -1.f, 1.f));
  m_upVector = glm::vec3(proj.getCurrent() * glm::vec4(0.f, 1.f, 0.f, 1.f));
  
  //Update the GL stack, for now (will need until switch completely to
  //shader-based rendering)
  proj.syncToGlStack(GL_PROJECTION);
}

void FirstPersonCamera::onPlayerMove(const glm::vec4 &old_pos, const glm::vec4 &new_pos)
{
  m_pos = new_pos;
  m_pos.y -= HEAD_Y;
}

void FirstPersonCamera::onPlayerLook(const Attitude &att)
{
  m_attitude.pitch = att.pitch;
  m_attitude.roll = att.roll;
  m_attitude.yaw = att.yaw;
}
