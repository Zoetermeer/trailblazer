#ifndef SOLAR_SYSTEM_H
#define SOLAR_SYSTEM_H

#include "common.hpp"
#include "process.hpp"
#include "scene-object.hpp"
#include "quaternion.hpp"
#include <string>
#include <vector>
#include <cmath>

typedef struct generator_params {
  int cube_size;
  int max_bands;
} generator_params_t;

enum class BodyType {
  Sun,
  Planet,
  Moon,
  AsteroidBelt
};

typedef struct asteroid {
  GLfloat radius;
  GLfloat rotation_speed;
  Quaternion local_rotation;
  Quaternion orbit_rotation;
  GLfloat dist_from_parent;
} asteroid_t;

typedef std::vector<class Body*>::iterator body_iter;

class Body : public SceneObject, public Process {
private:
  std::string m_name;
  BodyType m_type;
  GLfloat m_radius;
  Body *m_parent;
  int m_numChildren;
  std::vector<Body*> m_children;
  std::vector<asteroid_t> m_asteroids;
  GLfloat m_distFromParent;
  GLfloat m_rotationSpeed;
  GLfloat m_orbitSpeed;
  Quaternion m_orbitRotation;
  Quaternion m_localRotation;
  
public:
  Body()
  :Process(10000), m_numChildren(0), m_distFromParent(0.f), m_rotationSpeed(0.f), m_orbitSpeed(0.f)
  {
    
  }
  
  ~Body()
  {
    //Delete children
    body_iter citer;
    for (citer = m_children.begin(); citer != m_children.end(); ++citer) {
      delete *citer;
    }
  }
  
public:
  std::string getName() const { return m_name; }
  BodyType getType() const { return m_type; }
  GLfloat getRadius() const { return m_radius; }
  Body *getParent() const { return m_parent; }
  std::vector<Body*> *getChildren() { return &m_children; }
  GLfloat getDistFromParent() const { return m_distFromParent; }
  GLfloat getRotationSpeed() const { return m_rotationSpeed; }
  
  void draw(Env &env);
  void advance(int delta);
  bool isDone();
  
  friend class SolarSystem;
};


class SolarSystem : public SceneObject {
private:
  SolarSystem() { }
  
public:
  ~SolarSystem();
  
private:
  std::string m_name;
  Body *m_sun;
  
public:
  std::string getName() const { return m_name; }
  Body *getSun() const { return m_sun; }
  
public:
  static SolarSystem *generate(const generator_params_t &params);
  void animate();
  void draw(Env &env);
};

#endif