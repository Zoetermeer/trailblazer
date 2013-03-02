#ifndef SHIP_H
#define SHIP_H

#include <vector>
#include "common.hpp"
#include "scene-object.hpp"
#include "vertex-batch.hpp"

class MatrixStack;

typedef struct box {
  glm::vec3 model_space_origin;
  glm::vec3 dimensions;
  glm::vec3 normal;
  std::vector<struct box> surface;
} box_t;

class Ship : public SceneObject {
private:
  std::vector<box_t> m_geometry;
  vec3_t<GLfloat> m_rotAxis;
  
  VertexBatch *m_verts;
  GLfloat m_scale;
  
public:
  Ship(GLfloat scale)
  : m_scale(scale)
  {
    memset(&m_rotAxis, 0, sizeof(vec3_t<GLfloat>));
    this->generate();
  }
  
  Ship(GLfloat scale, GLfloat x, GLfloat y, GLfloat z)
  : Ship(scale)
  {
    position.x = x;
    position.y = y;
    position.z = z;
  }
  
  ~Ship()
  {
    delete m_verts;
  }
  
private:
  void generate();
  
public:
  Position position;
  Attitude attitude;
  void setRotAxis(GLfloat x, GLfloat y, GLfloat z)
  {
    m_rotAxis = { x, y, z };
  }
  
public:
  void draw(Env &env);
};

#endif