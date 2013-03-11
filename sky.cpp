#include "sky.hpp"
#include "shader.hpp"

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

#define SUN_HEIGHT 5000.f
void Sky::draw(Env &env)
{
  MatrixStack &mv = env.getMV();
  ShaderSet &shaders = env.getShaders();
  mv.pushMatrix();
  {
    //Always center the 'sky dome' on the player's (x,z) coordinates
    mv.translate(m_playerPos.x, 0.f, m_playerPos.z);
    mv.rotateZ(m_sunZRotation);
    
    glm::vec4 sunPos = toWorld();
    glm::vec4 skyColor = glm::mix(GL::BLACK, GL::color(135, 206, 250), (sunPos.y + SUN_HEIGHT) / (SUN_HEIGHT * 2));
    //shaders.prepareDefault(env, GL::color(135, 206, 250));
    shaders.prepareDefault(env, skyColor);
    glutSolidSphere(SUN_HEIGHT + 1000, 20, 20);
    
    mv.translateY(SUN_HEIGHT);
    
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