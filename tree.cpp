#include "tree.hpp"
#include "chunk.hpp"

void Tree::draw(Env &env)
{
  MatrixStack &mv = env.getMV();
  ShaderSet &shaders = env.getShaders();
  
  //shaders.preparePhong(env, GL::BLACK, GL::BLUE, GL::WHITE);
  //glutSolidCylinder(VOXEL_SIZE, VOXEL_SIZE * 3, 20, 1);
}