#ifndef QUATERNION_H
#define QUATERNION_H

#include <cmath>
#include "common.hpp"

class Quaternion {
public:
  Property x;
  Property y;
  Property z;
  Property w;
  
public:
  Quaternion() { }
  Quaternion(GLfloat w, GLfloat x, GLfloat y, GLfloat z)
  {
    this->w = w;
    this->x = x;
    this->y = y;
    this->z = z;
  }
  
  inline GLfloat magnitude()
  {
    return sqrt(w * w + x * x + y * y + z * z);
  }
  
  Quaternion normalize();
  Quaternion operator*(const Quaternion &b);
  Quaternion &operator*=(const Quaternion &b);
  void rotate(const Quaternion &rotation); 
  static Quaternion rotationQuaternion(GLfloat angle, vec3_t<GLfloat> axis);
};

#endif