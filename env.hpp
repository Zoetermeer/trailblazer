#ifndef ENV_H
#define ENV_H

class MatrixStack;
class ShaderSet;

class Env {
private:
  MatrixStack &m_mv;
  MatrixStack &m_proj;
  ShaderSet &m_shaders;
  
public:
  Env(MatrixStack &mv, MatrixStack &proj, ShaderSet &shaders)
  : m_mv(mv), m_proj(proj), m_shaders(shaders)
  {
    
  }
  
  ~Env()
  {
    
  }
  
public:
  MatrixStack &getMV() const { return m_mv; }
  MatrixStack &getProj() const { return m_proj; }
  ShaderSet &getShaders() const { return m_shaders; }
};

#endif