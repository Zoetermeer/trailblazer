#include "sky.hpp"

Sky *Sky::s_instance = NULL;

void Sky::init()
{
  s_instance = new Sky();
}

Sky &Sky::getInstance()
{
  return *s_instance;
}

glm::vec4 Sky::getSunPosition()
{
  return s_instance->toWorld();
}

void Sky::draw(Env &env)
{
  MatrixStack &mv = env.getMV();
  ShaderSet &shaders = env.getShaders();
  mv.pushMatrix();
  {
    //Always center the 'sky dome' on the player's (x,z) coordinates
    mv.translate(m_playerPos.x, 0.f, m_playerPos.z);
    mv.rotateZ(m_sunZRotation);
    
    shaders.prepareDefault(env, GL::color(135, 206, 250));
    glutSolidSphere(6000, 20, 20);
    
    mv.translateY(5000.f);
    
    //Update our world position
    m_world = mv.getCurrent();
    
    shaders.prepareDefault(env, GL::YELLOW);
    glutSolidSphere(500, 20, 20);
  }
  mv.popMatrix();
}

void Sky::advance(int deltaMs)
{
  m_sunZRotation = nextAngle(m_sunZRotation, .1f);
}

bool Sky::isDone()
{
  return false;
}

void Sky::onPlayerMove(const glm::vec4 &old_pos, const glm::vec4 &new_pos)
{
  
}