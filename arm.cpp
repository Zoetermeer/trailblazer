#include "arm.hpp"
#include "GL.hpp"
#include "animation.hpp"
#include "env.hpp"
#include "matrix-stack.hpp"
#include "shader.hpp"
#include "sky.hpp"

#define BODY_OFFSET 6.0

void Arm::onPlayerMove(const glm::vec4 &old_pos, const glm::vec4 &new_pos)
{
  this->m_pos = new_pos;
  m_world = glm::translate(m_world, glm::vec3(-m_pos.x, m_pos.y, -m_pos.z));
  m_world = glm::rotate(m_world, -m_playerAttitude.yaw, glm::vec3(0.f, 1.f, 0.f));
  m_world = glm::rotate(m_world, -m_playerAttitude.pitch, glm::vec3(1.f, 0.f, 1.f));
  m_world = glm::translate(m_world, glm::vec3(0.f, 0.f, -BODY_OFFSET));
}

void Arm::onPlayerLook(const Attitude &att)
{
  m_playerAttitude = att;
  m_world = glm::translate(m_world, glm::vec3(-m_pos.x, m_pos.y, -m_pos.z));
  m_world = glm::rotate(m_world, -m_playerAttitude.yaw, glm::vec3(0.f, 1.f, 0.f));
  m_world = glm::rotate(m_world, -m_playerAttitude.pitch, glm::vec3(1.f, 0.f, 1.f));
  m_world = glm::translate(m_world, glm::vec3(0.f, 0.f, -BODY_OFFSET));
}

void Arm::generateGeometry()
{
  GL::generateBoxWithSurfaceNoise(m_foreVerts, 4, false);
}

void Arm::draw(Env &env)
{
  MatrixStack &mv = env.getMV();
  ShaderSet &shaders = env.getShaders();
  auto sunPos = glm::vec3(Sky::getSunPosition());
  mv.pushMatrix();
  {
    mv.translate(m_pos.x, m_pos.y, m_pos.z);
    mv.rotateZ(m_playerAttitude.roll);
    mv.rotateX(m_playerAttitude.pitch);
    mv.rotateY(m_playerAttitude.yaw);
    
    mv.translateZ(-BODY_OFFSET);
    //mv.current() = m_world;
    mv.translateX(m_type == ArmType::Right ? BODY_OFFSET : -BODY_OFFSET);
    
    //Draw the upper arm
    mv.rotateY(armAttitude.yaw);
    mv.rotateX(armAttitude.pitch);
    mv.translateZ(-m_armLength);
    mv.pushMatrix();
    {
      mv.scaleZ(m_armLength);
      shaders.preparePhong(env, sunPos, GL::BLACK, GL::BLUE, GL::WHITE);
      GL::drawBox(GL_TRIANGLES);
    }
    mv.popMatrix();
    
    //Draw the forearm
    mv.translateZ(-m_armLength);
    mv.rotateY(forearmAttitude.yaw);
    mv.rotateZ(forearmAttitude.roll);
    mv.translateZ(-m_forearmLength);
    mv.pushMatrix();
    {
      mv.scaleZ(m_forearmLength);
      //shaders.preparePhong(env,
      //                     glm::vec4(0.329412, 0.223529, 0.027451, 1.0),
      //                     glm::vec4(0.780392, 0.568627, 0.113725, 1.0),
      //                     glm::vec4(0.992157, 0.941176, 0.807843, 1.0));
      shaders.prepareHemisphere(env, glm::vec3(0.f, 0.f, 0.f), GL::WHITE, GL::BLACK);
      m_foreVerts->draw(GL_TRIANGLES);
    }
    mv.popMatrix();
    
    //Draw the hand (a sphere)
    mv.translateZ(-m_forearmLength);
    
    //Joint position
    mv.translateZ(-m_handRadius);
    shaders.preparePhong(env, sunPos, GL::BLACK, GL::BLUE, GL::WHITE);
    glutSolidSphere(m_handRadius, 20, 20);
    
    //Draw fingers
    mv.pushMatrix();
    {
      mv.rotateY(120.f);
      for (int i = 0; i < 4; i++) {
        Finger &f = this->m_fingers[i];
        mv.rotateY(-40.f);
        
        mv.pushMatrix();
        {
          //Upper
          mv.translateZ(-m_handRadius);
          mv.rotateX(f.upper.pitch);
          mv.translateZ(-f.getLength() / 3.f);
          mv.pushMatrix();
          {
            mv.scale(0.2, 0.2, f.getLength() / 3.f);
            shaders.preparePhong(env, sunPos, GL::BLACK, GL::BLUE, GL::WHITE);
            GL::drawBox(GL_TRIANGLES);
          }
          mv.popMatrix();
          
          //Mid
          mv.translateZ(-f.getLength() / 3.f);
          mv.rotateX(f.middle.pitch);
          mv.translateZ(-f.getLength() / 3.f);
          
          mv.pushMatrix();
          {
            mv.scale(0.2, 0.2, f.getLength() / 3.f);
            shaders.preparePhong(env, sunPos, GL::BLACK, GL::BLUE, GL::WHITE);
            GL::drawBox(GL_TRIANGLES);
          }
          mv.popMatrix();
          
          //Lower
          mv.translateZ(-f.getLength() / 3.f);
          mv.rotateX(f.lower.pitch);
          mv.translateZ(-f.getLength() / 3.f);
          mv.pushMatrix();
          {
            mv.scale(0.2, 0.2, f.getLength() / 3.f);
            shaders.preparePhong(env, sunPos, GL::BLACK, GL::BLUE, GL::WHITE);
            GL::drawBox(GL_TRIANGLES);
          }
          mv.popMatrix();
        }
        mv.popMatrix();
      }
    }
    mv.popMatrix();
  }
  mv.popMatrix();
}

void Arm::swimForward()
{
  if (m_animating)
    return;
  
  if (m_lowered) {
    m_lowered = false;
    ProcessList::add(new Animation(Interpolation::Linear, armAttitude.pitch, ARM_LOWERED_PITCH, ARM_RAISE_LOWER_DURATION_MS));
    ProcessList::add(new ArmStrokeProcess(this, ARM_STROKE_DURATION_MS, ARM_RAISE_LOWER_DURATION_MS));
    return;
  }
  
  ProcessList::add(new ArmStrokeProcess(this, ARM_STROKE_DURATION_MS));
}

void Arm::lower()
{
  if (m_lowered)
    return;
  
  ProcessList::add(new Animation(Interpolation::Linear, armAttitude.pitch, -ARM_LOWERED_PITCH, ARM_RAISE_LOWER_DURATION_MS));
  m_lowered = true;
}

inline GLfloat ArmStrokeProcess::rotAng(GLfloat a)
{
  if (m_arm->getType() == ArmType::Right)
    return a;
  
  return -a;
}

void ArmStrokeProcess::advance(int delta)
{
  Process::advance(delta);
  unsigned int elapsed = getElapsedTime();
  if (elapsed == delta) {
    //First step
    unsigned int dur = getDuration();
    unsigned int outDelay = dur * .4;
    unsigned int retDelay = dur * .8;
    //In
    ProcessList::add(new Animation(Interpolation::Linear, m_arm->armAttitude.yaw, rotAng(5.f), dur * .4));
    ProcessList::add(new Animation(Interpolation::Linear, m_arm->forearmAttitude.yaw, rotAng(10.f), dur * .4));
    ProcessList::add(new Animation(Interpolation::Linear, m_arm->forearmAttitude.roll, rotAng(60.f), dur * .4));
    
    //Out
    ProcessList::add(new Animation(Interpolation::Linear, m_arm->armAttitude.yaw, rotAng(-5.f), dur * .4, outDelay));
    ProcessList::add(new Animation(Interpolation::Linear, m_arm->forearmAttitude.yaw, rotAng(-40.f), dur * .4, outDelay));
    
    ProcessList::add(new Animation(Interpolation::Linear, m_arm->forearmAttitude.yaw, rotAng(30.f), dur * .2, retDelay));
    ProcessList::add(new Animation(Interpolation::Linear, m_arm->forearmAttitude.roll, rotAng(-60.f), dur * .2, retDelay));
    for (int i = 0; i < 4; i++) {
      Finger &f = m_arm->getFinger(i);
      ProcessList::add(new Animation(Interpolation::Linear, f.upper.pitch, -10.f, dur * .4));
      ProcessList::add(new Animation(Interpolation::Linear, f.upper.pitch, 10.f, dur * .6, outDelay));
      
      ProcessList::add(new Animation(Interpolation::Linear, f.middle.pitch, 40.f, dur * .4));
      ProcessList::add(new Animation(Interpolation::Linear, f.middle.pitch, -40.f, dur * .6, outDelay));
      
      ProcessList::add(new Animation(Interpolation::Linear, f.lower.pitch, 30.f, dur * .4));
      ProcessList::add(new Animation(Interpolation::Linear, f.lower.pitch, -30.f, dur * .6, outDelay));
    }
  }
}

bool ArmStrokeProcess::isDone()
{
  bool done = Process::isDone();
  if (done) {
    m_arm->setIsAnimating(false);
    return true;
  }
  
  return false;
}

void ArmRaiseProcess::advance(int delta)
{
  Process::advance(delta);
  if (getElapsedTime() == delta) {
    ProcessList::add(new Animation(Interpolation::Linear, m_arm->armAttitude.pitch, ARM_LOWERED_PITCH, getDuration()));
  }
}

bool ArmRaiseProcess::isDone()
{
  bool done = Process::isDone();
  if (done) {
    m_arm->setIsLowered(false);
    return true;
  }
  
  return false;
}














