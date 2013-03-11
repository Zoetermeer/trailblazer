#include "player.hpp"
#include <math.h>
#include "events.hpp" 
#include <GL/glfw.h>

glm::vec3 Player::getLookVector() const
{
  glm::mat4 mat;
  mat = glm::translate(mat, glm::vec3(m_offset));
  mat = glm::rotate(mat, m_attitude.yaw.getValue(), glm::vec3(0.f, 1.f, 0.f));
  mat = glm::rotate(mat, m_attitude.pitch.getValue(), glm::vec3(1.f, 0.f, 1.f));
  glm::vec4 vec(0.f, 0.f, -1.f, 1.f);
  vec = mat * vec;
  return glm::vec3(vec.x, vec.y, vec.z) / vec.w;
}

void Player::mouseRoll(int oldX, int oldY, int newX, int newY)
{
  int xdelt = newX - oldX;
  m_attitude.roll = nextAngle(m_attitude.roll, xdelt);
  
  raiseLookEvent();
}

void Player::raiseLookEvent()
{
  Events::playerLookEvent({ m_attitude.roll, m_attitude.pitch, m_attitude.yaw }, getLookVector(), m_headlightOn);
}

void Player::mouseLook(int oldX, int oldY, int newX, int newY)
{
  int xdelt = oldX - newX, ydelt = oldY - newY;
  GLfloat new_pitch = nextAngle(m_attitude.pitch, ydelt * .1f);
  /* Bounds on the pitch of the player's head */
  if (new_pitch <= 70.0 || new_pitch >= 290.0)
    m_attitude.pitch = new_pitch;
  
  m_attitude.yaw = nextAngle(m_attitude.yaw, xdelt * .1f);
  raiseLookEvent();
}

#ifdef PRINT_PLAYER_LOCATION
#include <stdio.h>
#endif
void Player::move(GLfloat distance, Direction dir)
{
  GLfloat ang;
  GLfloat vertDist = 0.f;
  bool northSouth = false;
  glm::vec4 old = m_offset;
  switch (dir) 
  {
    case Direction::Forward:
      northSouth = true;
      break;
    case Direction::Backward:
      northSouth = true;
      distance = -distance;
      break;
    case Direction::Right:
      ang = nextAngle(m_attitude.yaw, -90.0);
      break;
    case Direction::Left:
      ang = nextAngle(m_attitude.yaw, 90.0);
      break;
    case Direction::Up:
      vertDist = distance;
      break;
    case Direction::Down:
      vertDist = -distance;
      break;
    case Direction::Idle:
      break;
  }

  if (northSouth) {
    m_heading = this->m_attitude.yaw;
    ang = m_heading;
  }

  if (vertDist != 0.f) {
    m_offset.y += vertDist;
  } else {
    m_offset.x += distance * -sin(DEG_TO_RAD(ang));
    m_offset.z -= distance * cos(DEG_TO_RAD(ang));
  }

#ifdef PRINT_PLAYER_LOCATION
  printf("pos: (%f, %f, %f), heading=%f, rotation=%f\n",
          m_offset.x,
          m_offset.y,
          m_offset.z, 
          m_heading, 
          m_attitude.yaw.getValue());
#endif
}

void Player::onKeyDown(int key, bool special)
{
  bool move = false;
  Direction dir;
  switch (key)
  {
    case 'w':
    case 'W':/* Up */
      m_moveDir = m_moveDir | Direction::Forward;
      //m_rightArm.swimForward();
      //m_leftArm.swimForward();
      break;
    case 's':
    case 'S': /* Down */
      m_moveDir = m_moveDir | Direction::Backward;
      break;
    case 'a':
    case 'A': /* Left */
      m_moveDir = m_moveDir | Direction::Left;
      //m_rightArm.swimForward();
      break;
    case 'd':
    case 'D': /* Right */
      m_moveDir = m_moveDir | Direction::Right;
      //m_leftArm.swimForward();
      break;
    case 'q':
    case 'Q':
      m_attitude.yaw = nextAngle(m_attitude.yaw, -5.0);
      break;
    case 'e':
    case 'E':
      m_attitude.yaw = nextAngle(m_attitude.yaw, 5.0);
      break;
    case GLFW_KEY_LSHIFT:
    case GLFW_KEY_RSHIFT:
      m_moveDir = m_moveDir | Direction::Down;
      break;
    case GLFW_KEY_SPACE:
      m_moveDir = m_moveDir | Direction::Up;
      break;
    case '.':
      m_headlightOn = !m_headlightOn;
      raiseLookEvent();
      break;
  }
}

void Player::onKeyUp(int key, bool special)
{
  switch (key)
  {
    case 'w':
    case 'W':
      m_moveDir = m_moveDir ^ Direction::Forward;
      break;
    case 's':
    case 'S':
      m_moveDir = m_moveDir ^ Direction::Backward;
      break;
    case 'a':
    case 'A':
      m_moveDir = m_moveDir ^ Direction::Left;
      break;
    case 'd':
    case 'D':
      m_moveDir = m_moveDir ^ Direction::Right;
      break;
    case GLFW_KEY_LSHIFT:
      m_moveDir = m_moveDir ^ Direction::Down;
      break;
    case GLFW_KEY_RSHIFT:
      m_moveDir = m_moveDir ^ Direction::Down;
      break;
    case GLFW_KEY_SPACE:
      m_moveDir = m_moveDir ^ Direction::Up;
      break;
  }
}

void Player::advance(int delta)
{
  glm::vec4 old_pos = m_offset;
  bool moved = false;
  const int MOVE_INTERVAL = 1.f;
  if ((m_moveDir & Direction::Forward) > Direction::Idle) {
    move(MOVE_INTERVAL, Direction::Forward);
    moved = true;
  }
  if ((m_moveDir & Direction::Backward) > Direction::Idle) {
    move(MOVE_INTERVAL, Direction::Backward);
    moved = true;
  }
  if ((m_moveDir & Direction::Right) > Direction::Idle) {
    move(MOVE_INTERVAL, Direction::Right);
    moved = true;
  }
  if ((m_moveDir & Direction::Left) > Direction::Idle) {
    move(MOVE_INTERVAL, Direction::Left);
    moved = true;
  }
  if ((m_moveDir & Direction::Up) > Direction::Idle) {
    move(MOVE_INTERVAL, Direction::Up);
    moved = true;
  }
  if ((m_moveDir & Direction::Down) > Direction::Idle) {
    move(MOVE_INTERVAL, Direction::Down);
    moved = true;
  }
  
  if (m_moveDir == Direction::Idle) {
    //m_rightArm.lower();
    //m_leftArm.lower();
  }
  
  //TODO: Collision detection here
  
  
  if (moved)
    Events::playerMoveEvent(old_pos, m_offset);
}

bool Player::isDone()
{
  return false;
}
