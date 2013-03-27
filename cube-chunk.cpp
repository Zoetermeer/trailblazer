#include "cube-chunk.hpp"
#include "matrix-stack.hpp"
#include "shader.hpp"
#include "GL.hpp"
#include "sky.hpp"
#include "units.hpp"
#include <noise.h>
#include "noiseutils.h"
#include <chrono>

#define VERT(m,x,y,z) (new_vertex((m * glm::vec4(x,y,z,1.f))))
#define NORMAL(a,b,c) (glm::normalize(glm::cross(glm::vec3(b.position - a.position), glm::vec3(c.position - a.position))))
void CubeChunk::addCube(const glm::vec3 &ind,
                        const Neighbors &ns,
                        const TerrainType &type,
                        VertexBatch *batch,
                        MatrixStack &stack)
{
  //Don't bother with totally occluded voxels
  if (ns == Neighbors::All)
    return;
  
  //Stat-keeping
  incrDrawnVoxels(1);
  
  glm::mat4 &m = stack.current();
  glm::vec4 color;
  switch (type)
  {
    case TerrainType::Grass:
      color = GL::color(0, 100, 0);
      break;
    case TerrainType::Dirt:
      color = GL::color(100, 69, 19);
      break;
    case TerrainType::Water:
      color = GL::BLUE;
      break;
    case TerrainType::Stone:
      color = GL::GRAY;
      break;
  }
  
  if ((ns & Neighbors::Left) == Neighbors::None) {
    float top = accessibilityAt(ind.x - 1, ind.y + 1, ind.z);
    float bot = accessibilityAt(ind.x - 1, ind.y - 1, ind.z);
    float lft = accessibilityAt(ind.x - 1, ind.y, ind.z - 1);
    float rgt = accessibilityAt(ind.x - 1, ind.y, ind.z + 1);
    float toprgt = accessibilityAt(ind.x - 1, ind.y + 1, ind.z + 1);
    float toplft = accessibilityAt(ind.x - 1, ind.y + 1, ind.z - 1);
    float botrgt = accessibilityAt(ind.x - 1, ind.y - 1, ind.z + 1);
    float botlft = accessibilityAt(ind.x - 1, ind.y - 1, ind.z - 1);
    
    auto v1 = VERT(m, -1, -1, -1);
    auto v2 = VERT(m, -1, -1, 1);
    auto v3 = VERT(m, -1, 1, 1);
    auto v4 = VERT(m, -1, 1, -1);
    auto nrm = NORMAL(v1, v2, v3);
    v1.normal = v2.normal = v3.normal = v4.normal = nrm;
    v1.color = v2.color = v3.color = v4.color = color;
    v1.voxel_coordinate = v2.voxel_coordinate = v3.voxel_coordinate = v4.voxel_coordinate = ind;
    v1.ao_accessibility = bot * lft * botlft;
    v2.ao_accessibility = bot * rgt * botrgt;
    v3.ao_accessibility = top * rgt * toprgt;
    v4.ao_accessibility = top * lft * toplft;
    
    batch->add(v1);
    batch->add(v2);
    batch->add(v3);
    batch->add(v4);
    
    //For stats/debugging
    incrVertexCount(4);
    
    batch->addIndex(v1.index);
    batch->addIndex(v2.index);
    batch->addIndex(v3.index);
    batch->addIndex(v1.index);
    batch->addIndex(v3.index);
    batch->addIndex(v4.index);
  }
  
  if ((ns & Neighbors::Back) == Neighbors::None) {
    float top = accessibilityAt(ind.x, ind.y + 1, ind.z - 1);
    float bot = accessibilityAt(ind.x, ind.y - 1, ind.z - 1);
    float lft = accessibilityAt(ind.x + 1, ind.y, ind.z - 1);
    float rgt = accessibilityAt(ind.x - 1, ind.y, ind.z - 1);
    float toplft = accessibilityAt(ind.x + 1, ind.y + 1, ind.z - 1);
    float toprgt = accessibilityAt(ind.x - 1, ind.y + 1, ind.z - 1);
    float botlft = accessibilityAt(ind.x + 1, ind.y - 1, ind.z - 1);
    float botrgt = accessibilityAt(ind.x - 1, ind.y - 1, ind.z - 1);
    
    auto v1 = VERT(m, 1, 1, -1);
    auto v2 = VERT(m, -1, -1, -1);
    auto v3 = VERT(m, -1, 1, -1);
    auto v4 = VERT(m, 1, -1, -1);
    v1.normal = v2.normal = v3.normal = v4.normal = NORMAL(v1, v2, v3);
    v1.color = v2.color = v3.color = v4.color = color;
    v1.voxel_coordinate = v2.voxel_coordinate = v3.voxel_coordinate = v4.voxel_coordinate = ind;
    v1.ao_accessibility = lft * top * toplft;
    v2.ao_accessibility = rgt * bot * botrgt;
    v3.ao_accessibility = rgt * top * toprgt;
    v4.ao_accessibility = lft * bot * botlft;
    
    batch->add(v1);
    batch->add(v2);
    batch->add(v3);
    batch->add(v4);
    
    //For stats/debugging
    incrVertexCount(4);
    
    batch->addIndex(v1.index);
    batch->addIndex(v2.index);
    batch->addIndex(v3.index);
    batch->addIndex(v1.index);
    batch->addIndex(v4.index);
    batch->addIndex(v2.index);
  }
  
  if ((ns & Neighbors::Bottom) == Neighbors::None) {
    auto v1 = VERT(m, 1, -1, 1);
    auto v2 = VERT(m, -1, -1, -1);
    auto v3 = VERT(m, 1, -1, -1);
    auto v4 = VERT(m, -1, -1, 1);
    v1.normal = v2.normal = v3.normal = v4.normal = NORMAL(v1, v2, v3);
    v1.color = v2.color = v3.color = v4.color = color;
    v1.ao_accessibility = v2.ao_accessibility = v3.ao_accessibility = v4.ao_accessibility = 1.f;
    v1.voxel_coordinate = v2.voxel_coordinate = v3.voxel_coordinate = v4.voxel_coordinate = ind;
    
    batch->add(v1);
    batch->add(v2);
    batch->add(v3);
    batch->add(v4);
    
    //For stats/debugging
    incrVertexCount(4);
    
    batch->addIndex(v1.index);
    batch->addIndex(v2.index);
    batch->addIndex(v3.index);
    batch->addIndex(v1.index);
    batch->addIndex(v4.index);
    batch->addIndex(v2.index);
  }
  
  if ((ns & Neighbors::Front) == Neighbors::None) {
    float top = accessibilityAt(ind.x, ind.y + 1, ind.z + 1);
    float bot = accessibilityAt(ind.x, ind.y - 1, ind.z + 1);
    float lft = accessibilityAt(ind.x - 1, ind.y, ind.z + 1);
    float rgt = accessibilityAt(ind.x + 1, ind.y, ind.z + 1);
    float botrgt = accessibilityAt(ind.x + 1, ind.y - 1, ind.z + 1);
    float botlft = accessibilityAt(ind.x - 1, ind.y - 1, ind.z + 1);
    float toprgt = accessibilityAt(ind.x + 1, ind.y + 1, ind.z + 1);
    float toplft = accessibilityAt(ind.x - 1, ind.y + 1, ind.z + 1);
    
    auto v1 = VERT(m, -1, 1, 1);
    auto v2 = VERT(m, -1, -1, 1);
    auto v3 = VERT(m, 1, -1, 1);
    auto v4 = VERT(m, 1, 1, 1);
    v1.normal = v2.normal = v3.normal = v4.normal = NORMAL(v1, v2, v3);
    v1.color = v2.color = v3.color = v4.color = color;
    v1.voxel_coordinate = v2.voxel_coordinate = v3.voxel_coordinate = v4.voxel_coordinate = ind;
    v1.ao_accessibility = lft * top * toplft;
    v2.ao_accessibility = lft * bot * botlft;
    v3.ao_accessibility = rgt * bot * botrgt;
    v4.ao_accessibility = rgt * top * toprgt;
    
    batch->add(v1);
    batch->add(v2);
    batch->add(v3);
    batch->add(v4);
    
    //For stats/debugging
    incrVertexCount(4);
    
    batch->addIndex(v1.index);
    batch->addIndex(v2.index);
    batch->addIndex(v3.index);
    batch->addIndex(v4.index);
    batch->addIndex(v1.index);
    batch->addIndex(v3.index);
  }
  
  if ((ns & Neighbors::Right) == Neighbors::None) {
    float top = accessibilityAt(ind.x + 1, ind.y + 1, ind.z);
    float bot = accessibilityAt(ind.x + 1, ind.y - 1, ind.z);
    float lft = accessibilityAt(ind.x + 1, ind.y, ind.z + 1);
    float rgt = accessibilityAt(ind.x + 1, ind.y, ind.z - 1);
    float toplft = accessibilityAt(ind.x + 1, ind.y + 1, ind.z + 1);
    float toprgt = accessibilityAt(ind.x + 1, ind.y + 1, ind.z - 1);
    float botlft = accessibilityAt(ind.x + 1, ind.y - 1, ind.z + 1);
    float botrgt = accessibilityAt(ind.x + 1, ind.y - 1, ind.z - 1);
    
    auto v1 = VERT(m, 1, 1, 1);
    auto v2 = VERT(m, 1, -1, -1);
    auto v3 = VERT(m, 1, 1, -1);
    auto v4 = VERT(m, 1, -1, 1);
    v1.normal = v2.normal = v3.normal = v4.normal = NORMAL(v1, v2, v3);
    v1.color = v2.color = v3.color = v4.color = color;
    v1.voxel_coordinate = v2.voxel_coordinate = v3.voxel_coordinate = v4.voxel_coordinate = ind;
    v1.ao_accessibility = top * lft * toplft;
    v2.ao_accessibility = bot * rgt * botrgt;
    v3.ao_accessibility = top * rgt * toprgt;
    v4.ao_accessibility = bot * lft * botlft;
    
    batch->add(v1);
    batch->add(v2);
    batch->add(v3);
    batch->add(v4);
    
    //For stats/debugging
    incrVertexCount(4);
    
    batch->addIndex(v1.index);
    batch->addIndex(v2.index);
    batch->addIndex(v3.index);
    batch->addIndex(v2.index);
    batch->addIndex(v1.index);
    batch->addIndex(v4.index);
  }
  
  if ((ns & Neighbors::Top) == Neighbors::None) {
    float lft = accessibilityAt(ind.x - 1, ind.y + 1, ind.z);
    float rgt = accessibilityAt(ind.x + 1, ind.y + 1, ind.z);
    float fnt = accessibilityAt(ind.x, ind.y + 1, ind.z + 1);
    float bck = accessibilityAt(ind.x, ind.y + 1, ind.z - 1);
    float fntlft = accessibilityAt(ind.x - 1, ind.y + 1, ind.z + 1);
    float fntrgt = accessibilityAt(ind.x + 1, ind.y + 1, ind.z + 1);
    float bcklft = accessibilityAt(ind.x - 1, ind.y + 1, ind.z - 1);
    float bckrgt = accessibilityAt(ind.x + 1, ind.y + 1, ind.z - 1);
    
    auto v1 = VERT(m, 1, 1, 1);
    auto v2 = VERT(m, 1, 1, -1);
    auto v3 = VERT(m, -1, 1, -1);
    auto v4 = VERT(m, -1, 1, 1);
    v1.normal = v2.normal = v3.normal = v4.normal = NORMAL(v1, v2, v3);
    v1.color = v2.color = v3.color = v4.color = color;
    v1.voxel_coordinate = v2.voxel_coordinate = v3.voxel_coordinate = v4.voxel_coordinate = ind;
    v1.ao_accessibility = rgt * fnt * fntrgt;
    v2.ao_accessibility = rgt * bck * bckrgt;
    v3.ao_accessibility = lft * bck * bcklft;
    v4.ao_accessibility = lft * fnt * fntlft;
    
    batch->add(v1);
    batch->add(v2);
    batch->add(v3);
    batch->add(v4);
    
    //For stats/debugging
    incrVertexCount(4);
    
    batch->addIndex(v1.index);
    batch->addIndex(v2.index);
    batch->addIndex(v3.index);
    batch->addIndex(v1.index);
    batch->addIndex(v3.index);
    batch->addIndex(v4.index);
  }
}

GLclampf CubeChunk::accessibilityAt(voxel_coord_type x, voxel_coord_type y, voxel_coord_type z)
{
  const unsigned VOXELS_PER_CHUNK = Units::chunkToVoxel(1);
  if (x < 0 || x > VOXELS_PER_CHUNK - 1)
    return 1.f;
  if (y < 0 || y > VOXELS_PER_CHUNK - 1)
    return 1.f;
  if (z < 0 || z > VOXELS_PER_CHUNK - 1)
    return 1.f;
  
  if (isVoxelActiveAt(x, y, z))
    return .9f;
  
  return 1.f;
}

//Safe to invoke on a worker thread
void CubeChunk::generateData()
{
  //Generate the height map
  Chunk::generateData();
  
  /* Generation strategy:
   We use the chunk's indices as bounds.
   So for any given chunk, we use noise values in the interval
   [chunk.x, chunk.x + 1], [0,1], [chunk.y, chunk.y + 1],
   where the 'y' is actually a z-index.
   We map the range of the noise function (which is in the interval
   [-1,1]) to a value in the range [0,31], where each voxel below this
   value is active and each one above is not active.
   Voxels are indexed starting at the bottom left corner of the chunk.
   In most cases, we only need a fraction of the total capacity of the chunk --
   all of the empty voxels can be discarded, as well as the totally occluded
   ones.  How do we initially store them, however (before determining which ones
   can be discarded)?
   */
  const unsigned VOXELS_PER_CHUNK = Units::chunkToVoxel(1);
  glm::ivec3 index = getIndex();
  auto vb = new VertexBatch();
  setVertexBuffer(vb);
  vertex_spec_t &spec = vb->getVertexSpec();
  spec.indexed = true;
  spec.use_ao = true;
  spec.use_color = true;
  spec.use_voxel_coordinates = true;
  vb->begin();
  
  const GLfloat VOXEL_SIZE = Units::voxelToGl(1);
  
  //Translate to the bottom left
  MatrixStack stack;
  GLfloat offset = VOXELS_PER_CHUNK * .5f - VOXEL_SIZE * .5f;
  stack.translate(-offset, -offset, offset);
  //Should now be at the origin of voxel (0,0,0)
  
  //First pass - enable/disable voxels using the height map
  //Can be parallelized if necessary
  for (voxel_coord_type i = 0; i < VOXELS_PER_CHUNK; i++) {
    for (voxel_coord_type j = 0; j < VOXELS_PER_CHUNK; j++) {
      //Calculate how many voxels are active in this column
      int ht = this->heightAt(i, j);
      
      //Set voxels active/inactive
      for (voxel_coord_type v = 0; v < VOXELS_PER_CHUNK; v++) {
        if (v < ht) {
          //Enable
          incrActiveVoxels(1);
          getVoxelMap()[hashCoords(i, v, j)] = true;
        } else {
          break;
        }
      }
    }
  }
  
  //Second pass -- figure out neighbors so
  //we can avoid drawing unnecessary vertices,
  //and assign voxel types
  const unsigned SEGMENT_SIZE = VOXELS_PER_CHUNK / PARALLEL_GENERATORS;
  std::vector<std::future<VertexBatch*>> tasks;
  for (unsigned p = 0; p < PARALLEL_GENERATORS; p++) {
    auto thunk = [=]() -> VertexBatch*
    {
      Neighbors ns = Neighbors::Bottom;
      TerrainType type;
      GLfloat noise;
      MatrixStack pstack(stack);
      VertexBatch *pbatch = new VertexBatch();
      pbatch->setVertexSpec(vb->getVertexSpec());
      pbatch->begin();
      unsigned ht;
      voxel_coord_type start = SEGMENT_SIZE * p;
      voxel_coord_type end = (SEGMENT_SIZE * p) + SEGMENT_SIZE;
      for (voxel_coord_type i = start; i < end; i++) {
        pstack.pushMatrix();
        {
          pstack.translateX(VOXEL_SIZE * i);
          for (voxel_coord_type j = 0; j < VOXELS_PER_CHUNK; j++) {
            //Calculate neighbors
            pstack.pushMatrix();
            {
              pstack.translateZ(VOXEL_SIZE * j);
              for (voxel_coord_type v = 0; v < VOXELS_PER_CHUNK; v++) {
                pstack.translateY(VOXEL_SIZE);
                noise = this->heightAt(i, j);
                ht = (VOXELS_PER_CHUNK * .5) * noise;
                ht += VOXELS_PER_CHUNK * .5;
                if (!ht) ht = 1;
                if (isVoxelActiveAt(i, v, j)) {
                  ns = Neighbors::Bottom;
                  if (v < VOXELS_PER_CHUNK - 1 && isVoxelActiveAt(i, v + 1, j)) {
                    ns = ns | Neighbors::Top;
                    type = TerrainType::Dirt;
                  } else if (v > 0) {
                    //Grass, unless we are at a low enough altitude
                    type = TerrainType::Grass;
                  } else {
                    type = TerrainType::Water;
                  }
                  
                  if (i > 0 && isVoxelActiveAt(i - 1, v, j))
                    ns = ns | Neighbors::Left;
                  if (i < VOXELS_PER_CHUNK - 1 && isVoxelActiveAt(i + 1, v, j))
                    ns = ns | Neighbors::Right;
                  if (j > 0 && isVoxelActiveAt(i, v, j - 1))
                    ns = ns | Neighbors::Back;
                  if (j < VOXELS_PER_CHUNK - 1 && isVoxelActiveAt(i, v, j + 1))
                    ns = ns | Neighbors::Front;
                  
                  pstack.pushMatrix();
                  {
                    pstack.scale(VOXEL_SIZE * .5f, VOXEL_SIZE * .5f, VOXEL_SIZE * .5f);
                    addCube(glm::vec3(i, v, j), ns, type, pbatch, pstack);
                  }
                  pstack.popMatrix();
                } else {
                  //We can't currently have any active voxels above an inactive
                  //one, so break
                  break;
                }
              }
            }
            pstack.popMatrix();
          }
        }
        pstack.popMatrix();
      }
      
      return pbatch;
    };
    
    //Spawn a future for this task
    auto task = std::async(std::launch::async, thunk);
    tasks.push_back(std::move(task));
  }
  
  //Combine results
  std::for_each(tasks.begin(), tasks.end(), [this, &vb](std::future<VertexBatch*> &ft)
  {
    ft.wait();
    VertexBatch *taskBatch = ft.get();
    vb->combineWith(*taskBatch);
    delete taskBatch;
  });
}








