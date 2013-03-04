#ifndef PLAYER_H
#define PLAYER_H

#include "common.hpp"
#include <type_traits>

enum class Direction {
  Idle = 0,
  Forward = 1,
  Backward = 2,
  Left = 4,
  Right = 8
};

inline Direction operator|(Direction a, Direction b)
{
  typedef std::underlying_type<Direction>::type enum_type;
  return static_cast<Direction>(static_cast<enum_type>(a) | static_cast<enum_type>(b));
}

inline Direction operator^(Direction a, Direction b)
{
  typedef std::underlying_type<Direction>::type enum_type;
  return static_cast<Direction>(static_cast<enum_type>(a) ^ static_cast<enum_type>(b));
}

inline Direction operator&(Direction a, Direction b)
{
  typedef std::underlying_type<Direction>::type enum_type;
  return static_cast<Direction>(static_cast<enum_type>(a) & static_cast<enum_type>(b));
}

/* The player's position can be 
   obtained by rotating world space 
   by 'heading' degrees, then 
   translating by 'offset'.
   'pitch' is the rotation angle about a Y-axis centered 
   on the player. 
   'yaw' is the rotation angle about an X-axis centered 
   on the player (assuming the player is looking down the 
   negative Z-axis). */
typedef struct player {
  glm::vec4 offset;
  GLfloat heading;
  GLfloat pitch;
  GLfloat roll;
  GLfloat yaw;
} player_t;

void move(player_t *player, GLfloat distance, Direction dir);

class Player {
private:
  glm::vec4 m_offset;
  GLfloat m_heading;
  Attitude m_attitude;
  
public:
  Player()
  {
    
  }
  
public:
  glm::vec4 getOffset() const { return m_offset; }
  void setOffset(const glm::vec4 v) { m_offset = v; }
  GLfloat getHeading() const { return m_heading; }
  Attitude &getAttitude() { return m_attitude; }

  void mouseRoll(int oldX, int oldY, int newX, int newY);
  void mouseLook(int oldX, int oldY, int newX, int newY);
  void move(GLfloat distance, Direction dir);
};

#endif
