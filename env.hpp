#ifndef ENV_H
#define ENV_H

#include "common.hpp"
#include "matrix-stack.hpp"
#include "player.hpp"
class ShaderSet;
class TextureSet;

class Env {
private:
  MatrixStack *m_mv;
  MatrixStack *m_proj;
  ShaderSet *m_shaders;
  TextureSet *m_textures;
  Player *m_player;
  glm::vec4 m_sunPos;
  debug_stats m_stats;
  
public:
  Env(ShaderSet &shaders, TextureSet &textures)
  {
    m_mv = new MatrixStack();
    m_proj = new MatrixStack();
    m_mv->loadIdentity();
    m_proj->loadIdentity();
    
    m_shaders = &shaders;
    m_textures = &textures;
    memset(&m_stats, 0, sizeof(debug_stats));
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
  TextureSet &getTextures() const { return *m_textures; }
  Player &getPlayer() const { return *m_player; }
  debug_stats &getStats() { return m_stats; }
  void setPlayer(Player *p) { m_player = p; };
  glm::vec4 getSunPos() const { return m_sunPos; }
  void setSunPos(glm::vec4 p) { m_sunPos = p; }
};

#endif