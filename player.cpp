#include "player.hpp"
#include <stdio.h>
#include <math.h>
#include "events.hpp" 

void Player::mouseRoll(int oldX, int oldY, int newX, int newY)
{
  int xdelt = newX - oldX;
  m_attitude.roll = nextAngle(m_attitude.roll, xdelt);
  
  Events::playerLookEvent({ m_attitude.roll, m_attitude.pitch, m_attitude.yaw });
}

void Player::mouseLook(int oldX, int oldY, int newX, int newY)
{
  int xdelt = oldX - newX, ydelt = oldY - newY;
  GLfloat new_pitch = nextAngle(m_attitude.pitch, ydelt);
  /* Bounds on the pitch of the player's head */
  if (new_pitch <= 70.0 || new_pitch >= 290.0)
    m_attitude.pitch = new_pitch;
  
  m_attitude.yaw = nextAngle(m_attitude.yaw, xdelt);  
  Events::playerLookEvent({ m_attitude.roll, m_attitude.pitch, m_attitude.yaw });
}

#undef PRINT_PLAYER_LOCATION
void Player::move(GLfloat distance, Direction dir)
{
  GLfloat ang;
  GLfloat dist;
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
    case Direction::Idle:
      break;
  }

  if (northSouth) {
    m_heading = this->m_attitude.yaw;
    ang = m_heading;
  }

  m_offset.x += distance * -sin(DEG_TO_RAD(ang));
  m_offset.z -= distance * cos(DEG_TO_RAD(ang));

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
    case 'w': /* Up */
      m_moveDir = m_moveDir | Direction::Forward;
      //m_rightArm.swimForward();
      //m_leftArm.swimForward();
      break;
    case 's': /* Down */
      m_moveDir = m_moveDir | Direction::Backward;
      break;
    case 'a': /* Left */
      m_moveDir = m_moveDir | Direction::Left;
      //m_rightArm.swimForward();
      break;
    case 'd': /* Right */
      m_moveDir = m_moveDir | Direction::Right;
      //m_leftArm.swimForward();
      break;
    case 'q':
      m_attitude.yaw = nextAngle(m_attitude.yaw, -5.0);
      break;
    case 'e':
      m_attitude.yaw = nextAngle(m_attitude.yaw, 5.0);
      break;
  }
}

void Player::onKeyUp(int key, bool special)
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
  
  if (m_moveDir == Direction::Idle) {
    //m_rightArm.lower();
    //m_leftArm.lower();
  }
  
  if (moved)
    Events::playerMoveEvent(old_pos, m_offset);
}

bool Player::isDone()
{
  return false;
}
