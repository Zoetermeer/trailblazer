#include "vertex-batch.hpp"
#include "GL.hpp"

void VertexBatch::begin()
{
  
}

void VertexBatch::add(glm::vec4 &vtx, glm::vec3 &nrm)
{
  add(vtx.x, vtx.y, vtx.z, nrm.x, nrm.y, nrm.z);
}

void VertexBatch::add(glm::vec3 vtx, GLfloat normX, GLfloat normY, GLfloat normZ)
{
  add(vtx.x, vtx.y, vtx.z, normX, normY, normZ);
}

void VertexBatch::add(GLfloat x, GLfloat y, GLfloat z, GLfloat normX, GLfloat normY, GLfloat normZ)
{
  m_verts.push_back(x);
  m_verts.push_back(y);
  m_verts.push_back(z);
  m_verts.push_back(1.0f);
  m_norms.push_back(normX);
  m_norms.push_back(normY);
  m_norms.push_back(normZ);
}

void VertexBatch::addIndex(GLuint index)
{
  m_indices.push_back(index);
}

void VertexBatch::addAOAccessibilityAttrib(float val)
{
  m_aoAttribs.push_back(val);
}

void VertexBatch::add(vertex_t &vertex)
{
  vertex.index = m_vertexSpec.next_index++;
  m_verts.push_back(vertex.position.x);
  m_verts.push_back(vertex.position.y);
  m_verts.push_back(vertex.position.z);
  m_verts.push_back(vertex.position.w);
  m_norms.push_back(vertex.normal.x);
  m_norms.push_back(vertex.normal.y);
  m_norms.push_back(vertex.normal.z);
  if (m_vertexSpec.use_color) {
    
  }
  
  if (m_vertexSpec.use_ao) {
    m_aoAttribs.push_back(vertex.ao_accessibility);
  }
}

void VertexBatch::end()
{
  GLfloat *verts = m_verts.data();
  GLfloat *norms = m_norms.data();
  
  glGenVertexArrays(1, &m_vaoId);
  glBindVertexArray(m_vaoId);
  
  if (m_vertexSpec.indexed) {
    GLuint ids[2];
    glGenBuffers(2, ids);
    m_vboId = ids[0];
    m_iboId = ids[1];
  } else {
    glGenBuffers(1, &m_vboId);
  }
  
  glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
  
  //Copy vertex data
  m_vertsSize = m_verts.size() * sizeof(GLfloat);
  m_normsSize = m_norms.size() * sizeof(GLfloat);
  m_indexSize = m_indices.size() * sizeof(GLuint);
  m_aoAttribsSize = m_aoAttribs.size() * sizeof(float);
  m_vertPtr = 0;
  m_normPtr = m_vertPtr + m_vertsSize;
  m_aoPtr = m_normPtr + m_normsSize;
  
  glBufferData(GL_ARRAY_BUFFER, m_vertsSize + m_normsSize + m_aoAttribsSize, NULL, GL_STATIC_DRAW);
  
  //Positions
  glBufferSubData(GL_ARRAY_BUFFER, m_vertPtr, m_vertsSize, verts);
  
  //Normals
  glBufferSubData(GL_ARRAY_BUFFER, m_normPtr, m_normsSize, norms);
  
  //If we have vertex attributes, store them as a sub-buffer
  if (m_aoAttribsSize) {
    float *accAtts = m_aoAttribs.data();
    glBufferSubData(GL_ARRAY_BUFFER, m_aoPtr, m_aoAttribsSize, accAtts);
  }
  
  glVertexPointer(4, GL_FLOAT, 0, (const GLvoid*)m_vertPtr);
  glNormalPointer(GL_FLOAT, 0, (const GLvoid*)m_normPtr);
  
  //Ambient occlusion (accessibility factor) vertex attribute
  if (m_vertexSpec.use_ao) {
    glEnableVertexAttribArray((GLuint)VertexAttrib::AOAccessibility);
    glVertexAttribPointer((GLuint)VertexAttrib::AOAccessibility,
                          1,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (const GLvoid*)m_aoPtr);
  }
  
  //Vertex element indices
  if (m_vertexSpec.indexed) {
    GLuint *inds = m_indices.data();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexSize, inds, GL_STATIC_DRAW);
  }
  
  CHECK_OPENGL_ERROR;
  glBindVertexArray(0);
  
  m_indices.clear();
}

void VertexBatch::draw(GLenum drawMode)
{
  glBindVertexArray(m_vaoId);
  glEnableClientState(GL_VERTEX_ARRAY);
  if (m_vertexSpec.indexed) {
    glDrawElements(drawMode, m_indexSize / sizeof(GLuint), GL_UNSIGNED_INT, 0);
  }
  else
    glDrawArrays(drawMode, 0, (m_vertsSize / sizeof(GLfloat)) / 4);
  
  glBindVertexArray(0);
  glDisableClientState(GL_VERTEX_ARRAY);
}













