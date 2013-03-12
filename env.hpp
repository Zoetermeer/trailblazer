#ifndef ENV_H
#define ENV_H

#include "matrix-stack.hpp"
#include "player.hpp"
class ShaderSet;

class Env {
private:
  MatrixStack *m_mv;
  MatrixStack *m_proj;
  ShaderSet *m_shaders;
  Player *m_player;
  glm::vec4 m_sunPos;
  
public:
  Env(ShaderSet &shaders)
  {
    m_mv = new MatrixStack();
    m_proj = new MatrixStack();
    m_mv->loadIdentity();
    m_proj->loadIdentity();
    
    m_shaders = &shaders;
  }
  
  ~Env()
  {
    delete m_mv;
    delete m_proj;
  }
  
public:
  MatrixStack &getMV() const { return *m_mv; }
  MatrixStack &getProj() const { return *m_proj; }
  ShaderSet &getShaders() const { return *m_shaders; }
  Player &getPlayer() const { return *m_player; }
  void setPlayer(Player *p) { m_player = p; };
  glm::vec4 getSunPos() const { return m_sunPos; }
  void setSunPos(glm::vec4 p) { m_sunPos = p; }
};

#endif