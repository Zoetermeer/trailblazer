#ifndef PLAYER_H
#define PLAYER_H

#include <type_traits>
#include "common.hpp"
#include "events.hpp"
#include "process.hpp"

enum class Direction {
  Idle = 0,
  Forward = 1,
  Backward = 2,
  Left = 4,
  Right = 8,
  Up = 16,
  Down = 32
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


class Player : public IKeyDownListener, public IKeyUpListener, public Process {
private:
  glm::vec4 m_offset;
  GLfloat m_heading;
  Attitude m_attitude;
  Direction m_moveDir;
  bool m_headlightOn;
  
public:
  Player()
  : m_moveDir(Direction::Idle), m_headlightOn(false)
  {
    ProcessList::add(this);
    Events::addListener((IKeyDownListener*)this);
    Events::addListener((IKeyUpListener*)this);
  }
  
  ~Player()
  {
    ProcessList::remove(this);
  }
  
private:
  void raiseLookEvent();
  
public:
  glm::vec4 getOffset() const { return m_offset; }
  void setOffset(const glm::vec4 v) { m_offset = v; }
  GLfloat getHeading() const { return m_heading; }
  Attitude &getAttitude() { return m_attitude; }
  bool getIsHeadlightOn() const { return m_headlightOn; }
  glm::vec3 getLookVector() const;
  
  void mouseRoll(int oldX, int oldY, int newX, int newY);
  void mouseLook(int oldX, int oldY, int newX, int newY);
  void move(GLfloat distance, Direction dir);
  
  //Key listener methods
  void onKeyDown(int key, bool special);
  void onKeyUp(int key, bool special);
  
  //Process methods
  void advance(int delta);
  bool isDone();
};

#endif
