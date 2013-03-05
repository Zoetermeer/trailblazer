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

void VertexBatch::addAOAccessibilityAttrib(float val)
{
  m_aoAttribs.push_back(val);
}

void VertexBatch::end()
{
  GLfloat *verts = m_verts.data();
  GLfloat *norms = m_norms.data();
  
  //glGenVertexArrays(1, &m_vaoId);
  //glBindVertexArray(m_vaoId);
  
  glGenBuffers(1, &m_vboId);
  glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
  
  //Copy vertex data
  m_vertsSize = m_verts.size() * sizeof(GLfloat);
  m_normsSize = m_norms.size() * sizeof(GLfloat);
  m_aoAttribsSize = m_aoAttribs.size() * sizeof(float);
  glBufferData(GL_ARRAY_BUFFER, m_vertsSize + m_normsSize + m_aoAttribsSize, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertsSize, verts);
  glBufferSubData(GL_ARRAY_BUFFER, m_vertsSize, m_normsSize, norms);
  if (m_aoAttribsSize) {
    float *accAtts = m_aoAttribs.data();
    glBufferSubData(GL_ARRAY_BUFFER, m_vertsSize + m_normsSize, m_aoAttribsSize, accAtts);
  }
  
  glVertexPointer(4, GL_FLOAT, 0, 0);
  glNormalPointer(GL_FLOAT, 0, (const GLvoid*)(m_verts.size() * sizeof(GLfloat)));
  //If we have vertex attributes, store them as a sub-buffer
  if (m_aoAttribsSize) {
    glEnableVertexAttribArray((GLuint)VertexAttrib::AOAccessibility);
    glVertexAttribPointer((GLuint)VertexAttrib::AOAccessibility,
                          1,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (const GLvoid*)(m_normsSize + m_vertsSize));
  }
  
  CHECK_OPENGL_ERROR;
}

void VertexBatch::draw(GLenum drawMode)
{
  glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(4, GL_FLOAT, 0, 0);
  glEnableClientState(GL_NORMAL_ARRAY);
  
  glNormalPointer(GL_FLOAT, 0, (const GLvoid*)m_vertsSize);
  if (m_aoAttribsSize) {
    glEnableVertexAttribArray((GLuint)VertexAttrib::AOAccessibility);
    glVertexAttribPointer((GLuint)VertexAttrib::AOAccessibility,
                          1,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (const GLvoid*)(m_vertsSize + m_normsSize));
  }
  
  glDrawArrays(drawMode, 0, m_verts.size() / 4);
}













