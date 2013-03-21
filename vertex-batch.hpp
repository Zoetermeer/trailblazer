#ifndef TRIANGLE_BATCH_H
#define TRIANGLE_BATCH_H

#include <vector>
#include "common.hpp"
#include "shader.hpp"
#include "vertex.hpp"

class VertexBatch {
private:
  std::vector<GLfloat> m_verts;
  std::vector<GLfloat> m_norms;
  std::vector<GLuint> m_indices;
  std::vector<GLfloat> m_colors;
  std::vector<GLfloat> m_aoAttribs;
  std::vector<GLfloat> m_vcAttribs;
  GLuint m_vaoId;
  GLuint m_vboId;
  GLuint m_iboId;
  
  vertex_spec_t m_vertexSpec;
  
  size_t m_vertPtr;
  size_t m_normPtr;
  size_t m_aoPtr;
  size_t m_indPtr;
  size_t m_colPtr;
  size_t m_vcPtr;
  
  size_t m_vertsSize;
  size_t m_normsSize;
  size_t m_indexSize;
  size_t m_colorSize;
  size_t m_aoAttribsSize;
  size_t m_vcAttribsSize;
  
  //Cache the largest index, in
  //case we need to combine with another vertex batch
  GLuint m_maxIndex;
  
public:
  VertexBatch()
  : m_vaoId(0),
    m_vboId(0),
    m_iboId(0),
    m_vertPtr(NULL),
    m_normPtr(NULL),
    m_aoPtr(NULL),
    m_indPtr(NULL),
    m_colPtr(NULL),
    m_vcPtr(NULL), 
    m_vertsSize(0),
    m_normsSize(0),
    m_indexSize(0),
    m_colorSize(0), 
    m_aoAttribsSize(0),
    m_vcAttribsSize(0),
    m_maxIndex(0)
  {
    memset(&m_vertexSpec, 0, sizeof(vertex_spec_t));
  }
  
  ~VertexBatch()
  {
    if (m_vboId)
      glDeleteBuffers(1, &m_vboId);
    
    if (m_iboId)
      glDeleteBuffers(1, &m_iboId);
    
    if (m_vaoId)
      glDeleteVertexArrays(1, &m_vaoId);
  }
  
public:
  vertex_spec_t &getVertexSpec()
  {
    return m_vertexSpec;
  }
  
  void setVertexSpec(vertex_spec_t &spec)
  {
    m_vertexSpec = spec;
  }
  
  GLuint getVaoId() const { return m_vaoId; }
  GLuint getVboId() const { return m_vboId; }
  GLuint getIboId() const { return m_iboId; }
  
public:
  void begin();
  void add(glm::vec4 &vtx, glm::vec3 &nrm);
  void add(glm::vec3 vtx, GLfloat normX, GLfloat normY, GLfloat normZ);
  void add(GLfloat x, GLfloat y, GLfloat z, GLfloat normX, GLfloat normY, GLfloat normZ);
  void add(vertex_t &vertex);
  void addIndex(GLuint index);
  void addAOAccessibilityAttrib(float val);
  void end();
  void draw(GLenum drawMode);
  
  void combineWith(const VertexBatch &b2);
};













#endif