#include "universe.hpp"

int Universe::s_voxelSize;

void Universe::insert(const vec3_t<int> &index, const time_t seed)
{
  
}

time_t Universe::lookup(const vec3_t<int> &index)
{
  //TODO: Lookup in some 3-dimensional-index data structure
  //...
  
  return 0;
}

void Universe::init(int voxelSize)
{
  s_voxelSize = voxelSize;
}

//TODO: Figure out how to delete solar system instances
//after the player leaves them, or when they're no longer visible
SolarSystem *Universe::getSystem(const vec3_t<int> &index, const generator_params_t &params)
{
  SolarSystem *sys;
  time_t seed = Universe::lookup(index);
  if (!seed) {
    seed = time(NULL);
  }
  
  return SolarSystem::generate(params);
}