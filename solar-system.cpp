#include "solar-system.hpp"
#include "matrix-stack.hpp"
#include "shader.hpp"
#include "env.hpp"
#include "GL.hpp"
#include <ctime>
#include <sstream>
#include <iostream>
#include <cstring>

void Body::draw(Env &env)
{
  MatrixStack &mv = env.getMV();
  ShaderSet &shaders = env.getShaders();
  mv.pushMatrix();
  {
    switch (m_type)
    {
      case BodyType::Sun:
      case BodyType::Planet:
      case BodyType::Moon:
      {
        mv.rotate(m_orbitRotation.w, m_orbitRotation.x, m_orbitRotation.y, m_orbitRotation.z);
        mv.translateX(m_distFromParent);
        mv.pushMatrix();
        {
          mv.rotate(m_localRotation.w, m_localRotation.x, m_localRotation.y, m_localRotation.z);
          shaders.disable();
          {
            //Not using a shader here, so update OpenGL's MV matrix
            glPushMatrix();
            {
              GL::setColor(GL::WHITE);
              mv.syncToGlStack(GL_MODELVIEW);
              GL::drawText(0.0, m_radius + 1.0, m_name.c_str());
            }
            glPopMatrix();
          }
        
          //Update our world position transform
          this->m_world = mv.getCurrent();
          if (m_type == BodyType::Sun)
            shaders.prepareDistortion(env, GL::RED, GL::YELLOW, GL::WHITE);
          else
            shaders.preparePhong(env, GL::BLUE, GL::BLACK, GL::WHITE);
        
          glutSolidSphere(m_radius, 32, 32);
          CHECK_OPENGL_ERROR;
        }
        mv.popMatrix();
        
        mv.rotate(-m_orbitRotation.w, m_orbitRotation.x, m_orbitRotation.y, m_orbitRotation.z);
        for (Body *child : m_children) {
          child->draw(env);
          
          //Draw a line to each child,
          //without a shader (so fall back to GL stack)
          mv.pushMatrix();
          {
            auto myWorld = this->toWorld();
            auto chWorld = child->toWorld();
            auto diff = chWorld - myWorld;
            mv.scale(diff.x, diff.y, diff.z);
            
            shaders.prepareDefault(env, GL::GRAY);
            GL::drawLine();
          }
          mv.popMatrix();
        }
      }
        break;
      case BodyType::AsteroidBelt:
        for (asteroid_t &ast : m_asteroids) {
          mv.pushMatrix();
          {
            mv.rotate(ast.orbit_rotation.w, ast.orbit_rotation.x, ast.orbit_rotation.y, ast.orbit_rotation.z);
            mv.translateX(ast.dist_from_parent);
            mv.rotate(ast.local_rotation.w, ast.local_rotation.x, ast.local_rotation.y, ast.local_rotation.z);
            //shaders.prepareDistortion(env,
            //                          glm::vec4(0.329412, 0.223529, 0.027451, 1.0),
            //                          glm::vec4(0.780392, 0.568627, 0.113725, 1.0),
            //                          glm::vec4(0.992157, 0.941176, 0.807843, 1.0));
            shaders.prepareHemisphere(env, glm::vec3(0.f, 100.f, 0.f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec4(0.0f, 0.0f, 0.3f, 1.f));
            glutSolidSphere(ast.radius, 20, 20);
          }
          mv.popMatrix();
        }
        break;
    }
  }
  mv.popMatrix();
}

void Body::advance(int delta)
{
  Process::advance(delta);
  if (delta == getElapsedTime()) {
    //Just started, need to add child processes
    for (Body *child : m_children) {
      ProcessList::add(child);
    }
  }
  
  switch (m_type)
  {
    case BodyType::AsteroidBelt:
      for (asteroid_t &ast : m_asteroids) {
        ast.local_rotation.w = nextAngle(ast.local_rotation.w, ast.rotation_speed);
        ast.orbit_rotation.w = nextAngle(ast.orbit_rotation.w, m_orbitSpeed);
      }
      break;
    default:
      this->m_localRotation.w = nextAngle(m_localRotation.w, m_rotationSpeed);
      this->m_orbitRotation.w = nextAngle(m_orbitRotation.w, m_orbitSpeed);
      break;
  }
}

bool Body::isDone()
{
  return false;
}

SolarSystem::~SolarSystem()
{
  delete m_sun;
}

#define RANDAXIS_CMP (randBetween(1.f, 50.f))
SolarSystem *SolarSystem::generate(const generator_params_t &params)
{
  int nplanets, i, j;
  Body *p, *m;
  SolarSystem *sys = new SolarSystem();
  
  srand(time(NULL));
  
  sys->m_name = randomName(true);
  
  //Sun
  Body *sun = new Body();
  sys->m_sun = sun;
  sun->m_type = BodyType::Sun;
  sun->m_radius = 10.0;
  sun->m_rotationSpeed = 1.0;
  sun->m_orbitSpeed = 1.0;
  sun->m_localRotation = Quaternion(0.f, random1(), random1(), random1());
  
  nplanets = randBetween(3, params.max_bands + 3);
  sun->m_numChildren = nplanets;
  GLfloat minDist = sun->m_radius + 10.f;
  for (i = 0; i < nplanets; i++) {
    p = new Body();
    sun->m_children.push_back(p);
    std::stringstream str;
    str << sys->m_name.c_str() << " " << i + 1;
    p->m_name = str.str();
    p->m_parent = sun;
    p->m_radius = randBetween(100.0, 700.0);
    minDist += p->m_radius + 500.f;
    p->m_distFromParent = randBetween(minDist, (GLfloat)params.cube_size);
    p->m_orbitSpeed = randBetween(.02f, .1f);
    p->m_orbitRotation = Quaternion(randAngle(), RANDAXIS_CMP, RANDAXIS_CMP, RANDAXIS_CMP);
    if (i == 2) {
      //Asteroid belt
      p->m_type = BodyType::AsteroidBelt;
      for (unsigned i = 0; i < 100; i++) {
        asteroid_t ast;
        memset(&ast, 0, sizeof(asteroid_t));
        ast.dist_from_parent = randBetween(minDist, p->m_radius);
        ast.local_rotation = Quaternion(randAngle(), RANDAXIS_CMP, RANDAXIS_CMP, RANDAXIS_CMP);
        ast.orbit_rotation = Quaternion(randAngle(), RANDAXIS_CMP, RANDAXIS_CMP, RANDAXIS_CMP);
        ast.rotation_speed = 2.f;
        //ast.radius = randBetween(3.f, 10.f);
        ast.radius = randBetween(20.f, 100.f);
        p->m_asteroids.push_back(ast);
      }
    } else {
      p->m_rotationSpeed = 0.05;
      p->m_localRotation = Quaternion(0.f, RANDAXIS_CMP, RANDAXIS_CMP, RANDAXIS_CMP);
      p->m_type = BodyType::Planet;
    }
    
    //Satellites
    p->m_numChildren = randBetween(1, 8);
    for (j = 0; j < p->m_numChildren; j++) {
      m = new Body();
      p->m_children.push_back(m);
      m->m_name = randomName(false);
      m->m_parent = p;
      m->m_distFromParent = randBetween(p->m_radius + 30.0, p->m_radius + 100.0);
      m->m_rotationSpeed = 0.1;
      m->m_orbitSpeed = random1() + .5f;
      m->m_orbitRotation = Quaternion(0.f, random1(), random1(), random1());
      m->m_localRotation = Quaternion(0.f, random1(), random1(), random1());
      m->m_radius = randBetween(10.f, 40.f);
      m->m_type = BodyType::Moon;
    }
  }
  
  return sys;
}

void SolarSystem::animate()
{
  ProcessList::add(new MaybeSpawnShipProcess());
  ProcessList::add(m_sun);
}

void SolarSystem::draw(Env &env)
{
  MatrixStack &mv = env.getMV();
  ShaderSet &shaders = env.getShaders();

  shaders.prepareDefault(env, GL::WHITE);
  GL::drawBackgroundStars();
  m_sun->draw(env);
}
















