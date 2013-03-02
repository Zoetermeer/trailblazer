#ifndef ANIMATION_H
#define ANIMATION_H

#include <cmath>
#include "common.hpp"
#include "process.hpp"

enum class Interpolation {
  Linear,
  QuadraticEaseIn,
  QuadraticEaseInOut,
  CubicEaseOut,
  CubicEaseIn,
  CubicEaseInOut,
  QuarticEaseOut,
  QuarticEaseIn,
  QuarticEaseInOut,
  QuinticEaseOut,
  QuinticEaseIn,
  QuinticEaseInOut,
  SinusoidalEaseOut,
  SinusoidalEaseIn,
  SinusoidalEaseInOut,
  ExponentialEaseOut,
  ExponentialEaseIn,
  ExponentialEaseInOut,
  CircularEaseOut,
  CircularEaseIn,
  CircularEaseInOut
};

GLfloat linearInterpolation(GLclampf t, GLfloat start, GLfloat end);
GLfloat quadraticEaseIn(GLclampf t, GLfloat start, GLfloat end);
GLfloat quadraticEaseInOut(GLclampf t, GLfloat start, GLfloat end);
GLfloat cubicEaseOut(GLclampf t, GLfloat start, GLfloat end);
GLfloat cubicEaseIn(GLclampf t, GLfloat start, GLfloat end);
GLfloat cubicEaseInOut(GLclampf t, GLfloat start, GLfloat end);
GLfloat quarticEaseOut(GLclampf t, GLfloat start, GLfloat end);
GLfloat quarticEaseIn(GLclampf t, GLfloat start, GLfloat end);
GLfloat quarticEaseInOut(GLclampf t, GLfloat start, GLfloat end);
GLfloat quinticEaseOut(GLclampf t, GLfloat start, GLfloat end);
GLfloat quinticEaseIn(GLclampf t, GLfloat start, GLfloat end);
GLfloat quinticEaseInOut(GLclampf t, GLfloat start, GLfloat end);
GLfloat sinusoidalEaseOut(GLclampf t, GLfloat start, GLfloat end);
GLfloat sinusoidalEaseIn(GLclampf t, GLfloat start, GLfloat end);
GLfloat sinusoidalEaseInOut(GLclampf t, GLfloat start, GLfloat end);
GLfloat exponentialEaseOut(GLclampf t, GLfloat start, GLfloat end);
GLfloat exponentialEaseIn(GLclampf t, GLfloat start, GLfloat end);
GLfloat exponentialEaseInOut(GLclampf t, GLfloat start, GLfloat end);
GLfloat circularEaseOut(GLclampf t, GLfloat start, GLfloat end);
GLfloat circularEaseIn(GLclampf t, GLfloat start, GLfloat end);
GLfloat circularEaseInOut(GLclampf t, GLfloat start, GLfloat end);

template <typename T>
T interpolate(Interpolation type, T start, T end, unsigned int duration, unsigned int elapsed)
{
  GLclampf normT = (GLfloat)elapsed / (GLfloat)duration;
  GLfloat (*f)(GLclampf, GLfloat, GLfloat);
  switch (type)
  {
    case Interpolation::Linear:
      f = &linearInterpolation;
      break;
    case Interpolation::QuadraticEaseIn:
      f = &quadraticEaseIn;
      break;
    case Interpolation::QuadraticEaseInOut:
      f = &quadraticEaseInOut;
      break;
    case Interpolation::CubicEaseOut:
      f = &cubicEaseOut;
      break;
    case Interpolation::CubicEaseIn:
      f = &cubicEaseIn;
      break;
    case Interpolation::CubicEaseInOut:
      f = &cubicEaseInOut;
      break;
    case Interpolation::QuarticEaseOut:
      f = &quarticEaseOut;
      break;
    case Interpolation::QuarticEaseIn:
      f = &quarticEaseIn;
      break;
    case Interpolation::QuarticEaseInOut:
      f = & quarticEaseInOut;
      break;
    case Interpolation::QuinticEaseOut:
      f = &quinticEaseOut;
      break;
    case Interpolation::QuinticEaseIn:
      f = &quinticEaseIn;
      break;
    case Interpolation::QuinticEaseInOut:
      f = & quinticEaseInOut;
      break;
    case Interpolation::SinusoidalEaseOut:
      f = &sinusoidalEaseOut;
      break;
    case Interpolation::SinusoidalEaseIn:
      f = & sinusoidalEaseIn;
      break;
    case Interpolation::SinusoidalEaseInOut:
      f = & sinusoidalEaseInOut;
      break;
    case Interpolation::ExponentialEaseOut:
      f = &exponentialEaseOut;
      break;
    case Interpolation::ExponentialEaseIn:
      f = &exponentialEaseIn;
      break;
    case Interpolation::ExponentialEaseInOut:
      f = &exponentialEaseInOut;
      break;
    case Interpolation::CircularEaseOut:
      f = &circularEaseOut;
      break;
    case Interpolation::CircularEaseIn:
      f = &circularEaseIn;
      break;
    case Interpolation::CircularEaseInOut:
      f = &circularEaseInOut;
      break;
  }
  
  return f(normT, start, end);
}

class Animation : public Process {
private:
  Interpolation m_type;
  Property &m_prop;
  GLfloat m_offset;
  GLfloat m_startValue;
  
public:
  Animation(Interpolation itype, Property &prop, GLfloat offset, unsigned int duration, unsigned int delay = 0)
  : Process(duration, delay), m_type(itype), m_prop(prop), m_offset(offset)
  {
    m_startValue = prop.getValue();
  }
  
public:
  void advance(int delta)
  {
    Process::advance(delta);
    unsigned int elapsed = getElapsedTime();
    if (!elapsed)
      return;
    
    //If this is the first step of the animation,
    //stash the starting value
    if (elapsed == delta)
      m_startValue = m_prop.getValue();
    
    m_prop.setValue(interpolate(m_type, m_startValue, m_startValue + m_offset, getDuration(), elapsed));
  }
};

#endif




















