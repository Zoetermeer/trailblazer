#ifndef UNIVERSE_H
#define UNIVERSE_H

#include "solar-system.hpp"
#include <ctime>

class Universe {
private:
  Universe() { }
  
private:
  static int s_voxelSize;
  
private:
  static time_t lookup(const vec3_t<int> &index);
  static void insert(const vec3_t<int> &index, const time_t seed);
  
public:
  static void init(int voxelSize);
  
  //'index' is the system's coordinates in voxels, or some
  //arbitrary subdivision of infinite space
  static SolarSystem *getSystem(const vec3_t<int> &index, const generator_params_t &params);
};

#endif