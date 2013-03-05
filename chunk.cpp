#include "chunk.hpp"
#include "matrix-stack.hpp"
#include "shader.hpp"
#include "GL.hpp"
#include <noise.h>
#include "noiseutils.h"

#define NOISE_INPUT_INCR (1.f / CHUNK_SIZE)

void Chunk::generate()
{
  if (m_generated)
    return;
  
  /* Generation strategy:
    We use the chunk's indices as bounds.
    So for any given chunk, we use noise values in the interval 
    [chunk.x, chunk.x + 1], [0,1], [chunk.y, chunk.y + 1], 
    where the 'y' is actually a z-index.
    We map the range of the noise function (which is in the interval 
    [-1,1] to a value in the range [0,31], where each voxel below this
    value is active and each one above is not active.
    Voxels are indexed starting at the bottom left corner of the chunk.
   */
  m_vbo = new VertexBatch();
  m_vbo->begin();
  
  //Translate to the bottom left
  MatrixStack stack;
  GLfloat offset = CHUNK_SIZE * .5f - VOXEL_SIZE * .5f;
  stack.translate(-offset, -offset, offset);
  //Should now be at the origin of voxel (0,0,0)
  
  noise::module::Perlin perlin;
  utils::NoiseMap heightMap;
  utils::NoiseMapBuilderPlane heightMapBuilder;
  heightMapBuilder.SetSourceModule(perlin);
  heightMapBuilder.SetDestNoiseMap(heightMap);
  heightMapBuilder.SetDestSize(32, 32);
  heightMapBuilder.SetBounds(m_chunkIndex.x,
                             m_chunkIndex.x + 1,
                             m_chunkIndex.y,
                             m_chunkIndex.y + 1);
  heightMapBuilder.Build();
  
  
  GLfloat x = 0.f, y = 0.f, z = 0.f;
  const GLfloat yincr = NOISE_INPUT_INCR * .5f;
  for (int i = 0; i < CHUNK_SIZE; i++) {
    stack.translateX(VOXEL_SIZE);
    x += NOISE_INPUT_INCR;
    stack.pushMatrix();
    {
      for (int j = 0; j < CHUNK_SIZE; j++) {
        stack.translateZ(VOXEL_SIZE);
        
        z += NOISE_INPUT_INCR;
        //GLfloat noise = perlin.GetValue(x, y, z);
        GLfloat noise = heightMap.GetValue(i, j);
        
        //Calculate how many voxels are active in this column
        int ht = (CHUNK_SIZE * .5) * noise;
        ht += CHUNK_SIZE * .5;
        y += yincr;
        if (!ht) ht = 1;
        
        //Set voxels active/inactive
        stack.pushMatrix();
        {
          for (int v = 0; v < CHUNK_SIZE; v++) {
            stack.translateY(VOXEL_SIZE);
            Voxel &voxel = m_voxels[i][v][j];
            voxel.setIndex(i, v, j);
            if (v < ht)
              voxel.setIsActive(true);
            else
              voxel.setIsActive(false);
            
            if (voxel.getIsActive()) {
              stack.pushMatrix();
              {
                stack.scale(VOXEL_SIZE * .5f, VOXEL_SIZE * .5f, VOXEL_SIZE * .5f);
                GL::addBox(m_vbo, stack, true, false);
              }
              stack.popMatrix();
            }
          }
        }
        stack.popMatrix();
      }
    }
    stack.popMatrix();
  }
  
  m_vbo->end();
  m_generated = true;
}

void Chunk::draw(Env &env)
{
  if (!m_generated)
    return;
  
  MatrixStack &mv = env.getMV();
  ShaderSet &shaders = env.getShaders();
  GLfloat offset = CHUNK_SIZE * VOXEL_SIZE;
  mv.pushMatrix();
  {
    mv.translate(m_chunkIndex.x * offset, 0.f, m_chunkIndex.y * offset);
    shaders.prepareHemisphere(env, glm::vec3(0.f, 100.f, 0.f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec4(0.0f, 0.0f, 0.3f, 1.f));
    m_vbo->draw(GL_TRIANGLES);
  }
  mv.popMatrix();
}









