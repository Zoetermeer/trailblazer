#include "poly-chunk.hpp"

GLclampf PolyChunk::accessibilityAt(voxel_coord_type x, voxel_coord_type y, voxel_coord_type z)
{
  return 1.f;
}

void PolyChunk::addCube(const glm::vec3 &ind,
                        const Neighbors &ns,
                        const TerrainType &type,
                        VertexBatch *batch,
                        MatrixStack &stack)
{
  
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
  spec.indexed = false;
  spec.use_ao = true;
  spec.use_color = true;
  spec.use_voxel_coordinates = true;
  vb->begin();
  
  //Do the naive thing first (vertex repetition without indexing)
  for (voxel_coord_type i = 0; i < VOXELS_PER_CHUNK; i++) {
    for (voxel_coord_type j = 0; j < VOXELS_PER_CHUNK; j++) {
      glm::vec3 vc(i, 0, j);
      GLfloat ht1 = heightAt(i, j);
      GLfloat ht2 = heightAt(i, j + 1);
      GLfloat ht3 = heightAt(i + 1, j);
      GLfloat ht4 = heightAt(i + 1, j + 1);
      //std::cout << "Height for (" << i << "," << j << ":" << ht1 << "," << ht2 << "," << ht3 << "," << ht4 << std::endl;
      
      vertex_t v1 = new_vertex(i * VOXEL_SIZE, ht1, j * VOXEL_SIZE);
      vertex_t v2a = new_vertex(i * VOXEL_SIZE, ht2, (j + 1) * VOXEL_SIZE);
      vertex_t v2b = new_vertex();
      v2b.position = v2a.position;
      vertex_t v3a = new_vertex((i + 1) * VOXEL_SIZE, ht3, j * VOXEL_SIZE);
      vertex_t v3b = new_vertex();
      v3b.position = v3a.position;
      vertex_t v4 = new_vertex((i + 1) * VOXEL_SIZE, ht4, (j + 1) * VOXEL_SIZE);
      
      glm::vec3 anormal = calc_normal(v1, v2a, v3a);
      glm::vec3 bnormal = calc_normal(v2b, v4, v3b);
      v1.normal = v2a.normal = v3a.normal = anormal;
      v2b.normal = v4.normal = v3b.normal = bnormal;
      
      //For now, make the land a uniform muted green
      v1.color = v2a.color = v3a.color = v2b.color = v4.color = v3b.color = GL::color(0, 100, 0);
      v1.ao_accessibility = v2a.ao_accessibility = v3a.ao_accessibility = 1.f;
      v2b.ao_accessibility = v4.ao_accessibility = v3b.ao_accessibility = 1.f;
      v1.voxel_coordinate = v2a.voxel_coordinate = v3a.voxel_coordinate = vc;
      v2b.voxel_coordinate = v4.voxel_coordinate = v3b.voxel_coordinate = vc;
      
      vb->add(v1);
      vb->add(v2a);
      vb->add(v3a);
      vb->add(v2b);
      vb->add(v4);
      vb->add(v3b);
      incrVertexCount(6);
    }
  }
}