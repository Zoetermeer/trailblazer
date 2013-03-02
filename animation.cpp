#include "animation.hpp"

GLfloat linearInterpolation(GLclampf t, GLfloat start, GLfloat end)
{
  return t * end + (1.f - t) * start;
}

GLfloat quadraticEaseOut(GLclampf t, GLfloat start, GLfloat end)
{
  return -end * t * (t - 2.f) -1.f;
}

GLfloat quadraticEaseIn(GLclampf t, GLfloat start, GLfloat end)
{
  //return end * t * t + start - 1.f;
  return end * t * t + start;
}

GLfloat quadraticEaseInOut(GLclampf t, GLfloat start, GLfloat end)
{
  t *= 2.f;
  //if (t < 1.f) return end / 2.f * t * t + start - 1.f;
  if (t < 1.f) return end / 2.f * t * t + start;
  t--;
  //return -end / 2.f * (t * (t - 2) - 1) + start - 1.f;
  return -end / 2.f * (t * (t - 2) - 1) + start;
}

GLfloat cubicEaseOut(GLclampf t, GLfloat start, GLfloat end)
{
  t--;
  return end*(t * t * t + 1.f) + start - 1.f;
}

GLfloat cubicEaseIn(GLclampf t, GLfloat start, GLfloat end)
{
  return end * t * t * t+ start - 1.f;
}

GLfloat cubicEaseInOut(GLclampf t, GLfloat start, GLfloat end)
{
  t *= 2.;
  if (t < 1.) return end/2 * t * t * t + start - 1.f;
  t -= 2;
  return end/2*(t * t * t + 2) + start - 1.f;
}

GLfloat quarticEaseOut(GLclampf t, GLfloat start, GLfloat end)
{
  t--;
  return -end * (t * t * t * t - 1) + start - 1.f;
}

GLfloat quarticEaseIn(GLclampf t, GLfloat start, GLfloat end)
{
  return end * t * t * t * t + start;
}

GLfloat quarticEaseInOut(GLclampf t, GLfloat start, GLfloat end)
{
  t *= 2.f;
  if (t < 1.f)
    return end / 2.f * t * t * t * t + start - 1.f;
  t -= 2.f;
  return -end / 2.f * (t * t * t * t - 2.f) + start - 1.f;
}

GLfloat quinticEaseOut(GLclampf t, GLfloat start, GLfloat end)
{
  t--;
  return end * (t * t * t * t * t + 1) + start - 1.f;
}

GLfloat quinticEaseIn(GLclampf t, GLfloat start, GLfloat end)
{
  return end * t * t * t * t * t + start - 1.f;
}

GLfloat quinticEaseInOut(GLclampf t, GLfloat start, GLfloat end)
{
  t *= 2.f;
  if (t < 1.f)
    return end/2 * t * t * t * t * t + start - 1.f;
  t -= 2;
  return end/2 * ( t * t * t * t * t + 2) + start - 1.f;
}

GLfloat sinusoidalEaseOut(GLclampf t, GLfloat start, GLfloat end)
{
  return end * sinf(t * (M_PI / 2)) + start - 1.f;
}

GLfloat sinusoidalEaseIn(GLclampf t, GLfloat start, GLfloat end)
{
  return -end * cosf(t * (M_PI / 2)) + end + start - 1.f;
}

GLfloat sinusoidalEaseInOut(GLclampf t, GLfloat start, GLfloat end)
{
  return -end / 2.f * (cosf(M_PI * t) - 1.f) + start - 1.f;
}

GLfloat exponentialEaseOut(GLclampf t, GLfloat start, GLfloat end)
{
  return end * (-powf(2.f, -10.f * t) + 1.f ) + start - 1.f;
}

GLfloat exponentialEaseIn(GLclampf t, GLfloat start, GLfloat end)
{
  return end * powf(2.f, 10.f * (t - 1.f) ) + start - 1.f;
}

GLfloat exponentialEaseInOut(GLclampf t, GLfloat start, GLfloat end)
{
  t *= 2.f;
  if (t < 1.f)
    return end/2.f * powf(2.f, 10.f * (t - 1.f) ) + start - 1.f;
  t--;
  return end/2.f * ( -powf(2.f, -10.f * t) + 2.f ) + start - 1.f;
}

GLfloat circularEaseOut(GLclampf t, GLfloat start, GLfloat end)
{
  t--;
  return end * sqrtf(1.f - t * t) + start - 1.f;
}

GLfloat circularEaseIn(GLclampf t, GLfloat start, GLfloat end)
{
  return -end * (sqrtf(1.f - t * t) - 1.f) + start - 1.f;
}

GLfloat circularEaseInOut(GLclampf t, GLfloat start, GLfloat end)
{
  t *= 2.f;
  if (t < 1.f)
    return -end/2.f * (sqrtf(1.f - t * t) - 1.f) + start - 1.f;
  t -= 2.f;
  return end/2.f * (sqrtf(1.f - t * t) + 1.f) + start - 1.f;
}