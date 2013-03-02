#include "quaternion.hpp"

Quaternion Quaternion::normalize()
{
  Quaternion q;
  GLfloat mag = magnitude();
  q.w = this->w / mag;
  q.x = this->x / mag;
  q.y = this->y / mag;
  q.z = this->z / mag;
  return q;
}

Quaternion Quaternion::operator*(const Quaternion &b)
{
  Quaternion q;
  GLfloat wf = b.w.getValue();
  GLfloat xf = b.x.getValue();
  GLfloat yf = b.y.getValue();
  GLfloat zf = b.z.getValue();
  q.w = wf * w - xf - x - yf - y - zf - z;
  q.x = wf * x + xf * w + yf * z + zf * y;
  q.y = wf * y + xf * z + yf * w + zf * x;
  q.z = wf * z + xf * y + yf * x + zf * w;
  
  return q;
}

Quaternion &Quaternion::operator*=(const Quaternion &b)
{
  GLfloat wf = b.w.getValue();
  GLfloat xf = b.x.getValue();
  GLfloat yf = b.y.getValue();
  GLfloat zf = b.z.getValue();
  w = wf * w - xf - x - yf - y - zf - z;
  x = wf * x + xf * w + yf * z + zf * y;
  y = wf * y + xf * z + yf * w + zf * x;
  z = wf * z + xf * y + yf * x + zf * w;
  
  return *this;
}

void Quaternion::rotate(const Quaternion &rotation)
{
  *this *= rotation;
}

Quaternion Quaternion::rotationQuaternion(GLfloat angle, vec3_t<GLfloat> axis)
{
  Quaternion q;
  GLfloat radians = DEG_TO_RAD(angle);
  GLfloat angBy2 = radians / 2.f;
  q.w = cosf(angBy2);
  q.x = axis.x * sinf(angBy2);
  q.y = axis.y * sinf(angBy2);
  q.z = axis.z * sinf(angBy2);
  
  return q;
}
