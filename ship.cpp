#include "ship.hpp"
#include "GL.hpp"
#include "matrix-stack.hpp"
#include "shader.hpp"
#include "env.hpp"

void Ship::generate()
{
  m_verts = new VertexBatch();
  GL::generateBoxWithSurfaceNoise(m_verts, 5, true);
}

void Ship::draw(Env &env)
{
  MatrixStack &mv = env.getMV();
  ShaderSet &shaders = env.getShaders();
  mv.pushMatrix();
  {
    mv.loadIdentity();
    mv.translate(position.x, position.y, position.z);
    mv.rotate(this->attitude.pitch, m_rotAxis.x, 0.f, m_rotAxis.z);
    m_world = mv.getCurrent();
    
    mv.scale(m_scale, m_scale, m_scale);
    shaders.prepareShip(env, glm::vec4(0.2, 0.2, 0.2, 1.0), GL::GRAY, GL::WHITE);
    //shaders.prepareHemisphere(env, glm::vec3(0.f, 0.f, 0.f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec4(0.2f, 0.2f, 0.2f, 1.f));
    m_verts->draw(GL_TRIANGLES);
  }
  mv.popMatrix();
}