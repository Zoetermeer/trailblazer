#ifndef TRIANGLE_BATCH_H
#define TRIANGLE_BATCH_H

#include <vector>
#include "common.hpp"
#include "shader.hpp"

class VertexBatch {
private:
  std::vector<GLfloat> m_verts;
  std::vector<GLfloat> m_norms;
  std::vector<float> m_aoAttribs;
  GLuint m_vaoId;
  GLuint m_vboId;
  
  size_t m_vertsSize;
  size_t m_normsSize;
  size_t m_aoAttribsSize;
  
public:
  VertexBatch()
  : m_vboId(0)
  {
    
  }
  
  ~VertexBatch()
  {
    if (m_vboId)
      glDeleteBuffers(1, &m_vboId);
  }
  
public:
  void begin();
  void add(glm::vec4 &vtx, glm::vec3 &nrm);
  void add(glm::vec3 vtx, GLfloat normX, GLfloat normY, GLfloat normZ);
  void add(GLfloat x, GLfloat y, GLfloat z, GLfloat normX, GLfloat normY, GLfloat normZ);
  void addAOAccessibilityAttrib(float val);
  void end();
  void draw(GLenum drawMode);
};













#endif