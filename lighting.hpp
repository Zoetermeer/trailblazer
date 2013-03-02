#ifndef LIGHTING_H
#define LIGHTING_H

#include "common.hpp"

class ILighting {
protected:
  ILighting() { }
  
public:
  virtual void init() = 0;
  virtual void enable() = 0;
  virtual void disable() = 0;
};

class DefaultAmbientLighting : public ILighting {
public:
  DefaultAmbientLighting() : ILighting() { }
  
public:
  void init();
  void enable();
  void disable();
};

#endif