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
