#ifndef UNITS_H
#define UNITS_H

#define MAX_HEIGHT_IN_FEET 2000.f

class Units {
public:
  static float noiseToFeet(GLclampf noise)
  {
    //return noise + 10.f;
    //return (noise + 1.f) * MAX_HEIGHT_IN_FEET;
    return noise * MAX_HEIGHT_IN_FEET;
  }
  
  static float glToFeet(GLfloat glDistance)
  {
    //Use a 1:1 size ratio between OpenGL coordinates
    //and feet (i.e. 1.0 in OGL coordinates is 1 foot).
    //Subject to change.
    return glDistance;
  }
  
  static float feetToGl(float feet)
  {
    return feet;
  }
  
  static float chunkToFeet(unsigned chunks)
  {
    return 5096.f * chunks;
  }
  
  static float chunkToGl(unsigned chunks)
  {
    return feetToGl(chunkToFeet(chunks));
  }
  
  static unsigned chunkToVoxel(unsigned chunks)
  {
    return chunkToFeet(chunks) / voxelToFeet(1);
  }
  
  static float voxelToGl(unsigned voxels)
  {
    return 128.f;
  }
  
  static float feetToLat(float feet)
  {
    return feet / 362800.f;
  }
  
  static float voxelToFeet(unsigned voxels)
  {
    return glToFeet(voxelToGl(voxels));
  }
  
  static glm::ivec3 worldToVoxel(const glm::vec4 &world)
  {
    return glm::ivec3((int)world.x, (int)world.y, -((int)world.z));
  }
  
  static glm::vec2 worldToLatLon(const glm::vec4 &world)
  {
    return voxelToLatLon(worldToVoxel(world));
  }
  
  static glm::vec2 voxelToLatLon(const glm::ivec3 &vc)
  {
    return glm::vec2(feetToLat(voxelToFeet(vc.x)),
                     feetToLat(voxelToFeet(vc.z)));
  }
};

#endif