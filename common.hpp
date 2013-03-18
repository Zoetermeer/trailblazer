#ifndef COMMON_H
#define COMMON_H

#include <glut/glut.h>
#include <stdlib.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <string>
#include <iostream>

#define DEFAULT_UPDATE_INTERVAL (1000 / 60)

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

#define DEG_TO_RAD(d) (d * 0.0174532925)
#define RAD_TO_DEG(r) (r * (180.0 / M_PI))

typedef unsigned char byte;

#define INIT(type, ptr) \
ptr = (type*)malloc(sizeof(type)); \
memset(ptr, 0, sizeof(type));

#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#define glVertexAttribIPointer glVertexAttribIPointerEXT

template <typename T, int dimension>
struct vec_t {
  T v[dimension];
};

template<typename T>
struct vec4_t {
  T w;
  T x;
  T y;
  T z;
};

template <typename T>
struct vec3_t {
  T x;
  T y;
  T z;
};

template <typename T>
struct vec2_t {
  T x;
  T y;
};

//length = sqrt((ax * ax) + (ay * ay) + (az * az))
//length = sqrt(9 + 1 + 4) = 3.742
template<typename T>
inline T magnitude(const vec3_t<T> &vec)
{
  return sqrt(vec.x * vec.x) + sqrt(vec.y * vec.y) + sqrt(vec.z + vec.z);
}

template<typename T>
inline vec3_t<T> normalize(const vec3_t<T> &vec)
{
  vec3_t<T> n;
  T mag = magnitude<T>(vec);
  n.x = vec.x / mag;
  n.y = vec.y / mag;
  n.z = vec.z / mag;
}

template<typename T>
inline T dot(const vec3_t<T> &a, const vec3_t<T> &b)
{
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

//GLfloat angleBetween(const vec3_t<GLfloat> &a, const vec3_t<GLfloat> &b);

class Property {
private:
  GLfloat m_value;
  
public:
  Property(GLfloat val = 0.f)
  {
    m_value = val;
  }
  
  Property(const Property &other)
  {
    m_value = other.getValue();
  }
  
  void setValue(GLfloat v) { m_value = v; }
  GLfloat getValue() const { return m_value; }
  
  Property& operator=(const Property &other)
  {
    //Check for self-assignment
    if (this == &other)
      return *this;
    
    this->setValue(other.getValue());
    return *this;
  }
  
  Property& operator=(const GLfloat v)
  {
    this->setValue(v);
    return *this;
  }
  
  inline GLfloat operator*(const Property &other) const
  {
    return m_value * other.getValue();
  }
  
  inline GLfloat operator+(const Property &other) const
  {
    return m_value + other.getValue();
  }
  
  inline Property &operator+=(GLfloat v)
  {
    m_value += v;
    return *this;
  }
  
  operator GLfloat()
  {
    return this->m_value;
  }
};

inline static GLfloat operator*(const GLfloat a, const Property &b)
{
  return a * b.getValue();
}

class Attitude {
public:
  Attitude(GLfloat r, GLfloat p, GLfloat y)
  {
    roll.setValue(r);
    pitch.setValue(p);
    yaw.setValue(y);
  }
  
  Attitude()
  :Attitude(0.f, 0.f, 0.f)
  {
    
  }
  
  ~Attitude()
  {
  }
  
public:
  Property roll;
  Property pitch;
  Property yaw;
};

class Position {
public:
  Position()
  : Position(0.f, 0.f, 0.f)
  {
    
  }
  
  Position(GLfloat fx, GLfloat fy, GLfloat fz)
  {
    x.setValue(fx);
    y.setValue(fy);
    z.setValue(fz);
  }
  
public:
  Property x;
  Property y;
  Property z;
};

/* A random real between 0 and 1 */
double random1();
double randBetween(double min, double max);
int randBetween(int min, int max);
GLfloat randAngle();
bool percentChance(float pct);
std::string randomName(bool upper);
char randVowel(bool upper);
char randConsonant(bool upper);
GLfloat nextAngle(GLfloat cur, GLfloat incr);
glm::vec3 randVec3(GLfloat min, GLfloat max);

#endif
