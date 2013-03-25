#include "poly-chunk.hpp"
#include "units.hpp"

GLclampf PolyChunk::accessibilityAt(voxel_coord_type x, voxel_coord_type y, voxel_coord_type z)
{
  return 1.f;
}

glm::vec3 PolyChunk::normalForQuad(voxel_coord_type x, voxel_coord_type z)
{
  const GLfloat VSZ = Units::voxelToGl(1);
  glm::vec4 v1(x * VSZ, heightAt(x, z), z * VSZ, 1.f);
  glm::vec4 v2(x * VSZ, heightAt(x, z + 1), (z + 1) * VSZ, 1.f);
  glm::vec4 v3((x + 1) * VSZ, heightAt(x + 1, z + 1), (z + 1) * VSZ, 1.f);
  return calc_normal(v1, v2, v3);
}

glm::vec3 PolyChunk::vertexNormal(byte localIndex, glm::vec3 quadNorm, voxel_coord_type vx, voxel_coord_type vz)
{
  switch (localIndex)
  {
    case 1:
      return glm::normalize(normalForQuad(vx - 1, vz - 1) +
                            normalForQuad(vx - 1, vz) +
                            quadNorm +
                            normalForQuad(vx, vz - 1));
    case 2:
      return glm::normalize(normalForQuad(vx - 1, vz) +
                            normalForQuad(vx - 1, vz + 1) +
                            normalForQuad(vx, vz + 1) +
                            quadNorm);
    case 3:
      return glm::normalize(normalForQuad(vx, vz - 1) +
                            quadNorm +
                            normalForQuad(vx + 1, vz) +
                            normalForQuad(vx + 1, vz - 1));
    default:
      return glm::normalize(quadNorm +
                            normalForQuad(vx, vz + 1) +
                            normalForQuad(vx + 1, vz + 1) +
                            normalForQuad(vx + 1, vz));
  }
}

void PolyChunk::generateData()
{
  Chunk::generateData();
  
  /* 
   Strategy: 
   We create a mesh with VOXELS_PER_CHUNK * VOXELS_PER_CHUNK quads, 
   where quad edges align with voxel edges -- and each quad 
   is actually 2 triangles.  So we have a 2d grid of vertices, and we 
   displace the vertex's Y by a (scaled) height-map value.  This value 
   is heightmap[i, j], where i,j is the 2d index of the vertex.
   How do we calculate surface normals?  Without this we could do the 
   displacement in the vertex shader, but we need all three vertices from 
   each triangle to do normal calculation.
   */
  auto vb = new VertexBatch();
  setVertexBuffer(vb);
  vertex_spec_t &spec = vb->getVertexSpec();
  spec.indexed = true;
  spec.use_ao = true;
  spec.use_color = true;
  spec.use_voxel_coordinates = true;
  vb->begin();
  
  //Do the naive thing first (vertex repetition without indexing)
  const GLfloat VSZ = Units::voxelToGl(1);
  for (voxel_coord_type i = 0; i < VOXELS_PER_CHUNK; i++) {
    for (voxel_coord_type j = 0; j < VOXELS_PER_CHUNK; j++) {
      incrActiveVoxels(1);
      incrDrawnVoxels(1);
      
      glm::vec3 vc(i, 0, j);
      GLfloat ht1 = heightAt(i, j);
      GLfloat ht2 = heightAt(i, j + 1);
      GLfloat ht3 = heightAt(i + 1, j);
      GLfloat ht4 = heightAt(i + 1, j + 1);
      
      vertex_t v1 = new_vertex(i * VSZ, ht1, j * VSZ);
      vertex_t v2 = new_vertex(i * VSZ, ht2, (j + 1) * VSZ);
      vertex_t v3 = new_vertex((i + 1) * VSZ, ht3, j * VSZ);
      vertex_t v4 = new_vertex((i + 1) * VSZ, ht4, (j + 1) * VSZ);
      
      //Calculate each vertex's normal as a function of its neighboring quads
      glm::vec3 quadNorm = calc_normal(v1, v2, v3);
      v1.normal = vertexNormal(1, quadNorm, i, j);
      v2.normal = vertexNormal(2, quadNorm, i, j);
      v3.normal = vertexNormal(3, quadNorm, i, j);
      v4.normal = vertexNormal(4, quadNorm, i, j);
      
      glm::vec4 color = containsPlayer() ? GL::RED : GL::color(0, 100, 0);
      v1.color = v2.color = v3.color = v4.color = color;
      v1.ao_accessibility = v2.ao_accessibility = v3.ao_accessibility = v4.ao_accessibility = 1.f;
      v1.voxel_coordinate = v2.voxel_coordinate = v3.voxel_coordinate = v4.voxel_coordinate = vc;
      
      vb->add(v1);
      vb->add(v2);
      vb->add(v3);
      vb->add(v4);
      
      vb->addIndex(v1.index);
      vb->addIndex(v2.index);
      vb->addIndex(v3.index);
      vb->addIndex(v2.index);
      vb->addIndex(v4.index);
      vb->addIndex(v3.index);
      incrVertexCount(4);
    }
  }
}