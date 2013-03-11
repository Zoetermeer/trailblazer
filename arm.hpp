#ifndef ARM_H
#define ARM_H

#include "common.hpp"
#include "process.hpp"
#include "events.hpp"
#include "vertex-batch.hpp"
#include "scene-object.hpp"

#define ARM_LOWERED_PITCH 90.f
#define ARM_RAISE_LOWER_DURATION_MS 500
#define ARM_STROKE_DURATION_MS 1000

class MatrixStack;
class ShaderProgram;

enum class ArmType {
  Right,
  Left
};

class Finger {
private:
  GLfloat m_len;
public:
  Finger()
  {
    
  }
  
  Finger(GLfloat len)
  : m_len(len)
  {
    
  }
  
  Attitude upper;
  Attitude middle;
  Attitude lower;
  void setLength(GLfloat v) { m_len = v; }
  GLfloat getLength() const { return m_len; }
};

class Arm : public SceneObject, public IPlayerMoveListener, public IPlayerLookListener {
private:
  ArmType m_type;
  bool m_animating;
  bool m_lowered;
  glm::vec4 m_pos;
  Attitude m_playerAttitude;
  GLfloat m_armLength;
  GLfloat m_forearmLength;
  GLfloat m_handRadius;
  Finger m_fingers[4];
  
  VertexBatch *m_foreVerts;
  
public:
  Arm(ArmType type, GLfloat armLen, GLfloat forearmLen, GLfloat handRad)
  : SceneObject(), m_type(type), m_animating(false), m_lowered(true), m_armLength(armLen), m_forearmLength(forearmLen), m_handRadius(handRad)
  {
    m_foreVerts = new VertexBatch();
    
    Events::addListener((IPlayerMoveListener*)this);
    Events::addListener((IPlayerLookListener*)this);
    
    //Start the arm out of view
    armAttitude.pitch = -ARM_LOWERED_PITCH;

    //Finger default positions/dimensions
    for (int i = 0; i < 4; i++) {
      Finger &f = m_fingers[i];
      f.setLength(3.0);
      f.upper.pitch = -10.f;
      f.middle.pitch = -30.f;
      f.lower.pitch = -40.f;
    }
  }
  
  ~Arm()
  {
    delete m_foreVerts;
    Events::removeListener(EventPlayerMove, this);
    Events::removeListener(EventPlayerLook, this);
  }
  
public:
  Attitude armAttitude;
  Attitude forearmAttitude;
  Attitude handAttitude;
  ArmType getType() const { return m_type; }
  glm::vec4 getPos() const { return m_pos; }
  bool getIsAnimating() const { return m_animating; }
  Attitude getPlayerAttitude() const { return m_playerAttitude; }
  GLfloat getArmLength() const { return m_armLength; }
  GLfloat getForearmLength() const { return m_forearmLength; }
  GLfloat getHandRadius() const { return m_handRadius; }
  Finger &getFinger(int index) { return m_fingers[index]; }
  
  void setIsAnimating(bool v) { m_animating = v; }
  void setIsLowered(bool v) { m_lowered = v; }
  
public:
  void generateGeometry();
  void draw(Env &env);
  void onPlayerMove(const glm::vec4 &old_pos, const glm::vec4 &new_pos);
  void onPlayerLook(const Attitude &att);
  void swimForward();
  void lower();
  
  friend class ArmStrokeProcess;
};

enum class SwingState {
  StrokeIn,
  StrokeOut,
  Return
};

class ArmStrokeProcess : public Process {
private:
  Arm *m_arm;
  
public:
  ArmStrokeProcess(Arm *arm, unsigned int duration, unsigned int delay = 0)
  : Process(duration, delay)
  {
    m_arm = arm;
    arm->setIsAnimating(true);
  }
  
private:
  inline GLfloat rotAng(GLfloat a);
  
public:
  void advance(int delta);
  bool isDone();
};

class ArmRaiseProcess : public Process {
private:
  Arm *m_arm;
  
public:
  ArmRaiseProcess(Arm *arm, unsigned int duration)
  : Process(duration)
  {
    m_arm = arm;
    arm->setIsAnimating(true);
  }
  
public:
  void advance(int delta);
  bool isDone();
};

#endif