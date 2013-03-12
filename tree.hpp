#ifndef TREE_H
#define TREE_H

#include "scene-object.hpp"
#include "env.hpp"

class Tree : public SceneObject {
public:
  Tree()
  {
    
  }
  
  ~Tree()
  {
    
  }
  
public:
  virtual void draw(Env &env);
};

#endif