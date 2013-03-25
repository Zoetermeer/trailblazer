#ifndef UNITS_H
#define UNITS_H

#define MAX_HEIGHT_IN_FEET 500.f

class Units {
public:
  static float noiseToFeet(GLclampf noise)
  {
    //return noise + 10.f;
    return (noise + 1.f) * MAX_HEIGHT_IN_FEET;
  }
  
  static float glToFeet(GLfloat glDistance)
  {
    //Use a 1:1 size ratio between OpenGL coordinates
    //and feet (i.e. 1.0 in OGL coordinates is 1 foot).
    //Subject to change.
    return glDistance;
  }
  
  static float voxelToGl(unsigned voxels)
  {
    return voxels * 100.f;
    //return (float)voxels;
  }
  
  static float feetToLat(float feet)
  {
    return feet / 362800.f;
  }
  
  static float voxelToFeet(unsigned voxels)
  {
    return (float)voxels;
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