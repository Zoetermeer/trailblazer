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

void VertexBatch::end()
{
  GLfloat *verts = m_verts.data();
  GLfloat *norms = m_norms.data();
  
  //glGenVertexArrays(1, &m_vaoId);
  //glBindVertexArray(m_vaoId);
  
  glGenBuffers(1, &m_vboId);
  glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
  
  //Copy vertex data
  size_t vertsz = m_verts.size() * sizeof(GLfloat);
  size_t normsz = m_norms.size() * sizeof(GLfloat);
  glBufferData(GL_ARRAY_BUFFER, vertsz + normsz, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertsz, verts);
  glBufferSubData(GL_ARRAY_BUFFER, vertsz, normsz, norms);
  glVertexPointer(4, GL_FLOAT, 0, 0);
  glNormalPointer(GL_FLOAT, 0, (const GLvoid*)(m_verts.size() * sizeof(GLfloat)));
  CHECK_OPENGL_ERROR;
}

void VertexBatch::draw(GLenum drawMode)
{
  glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(4, GL_FLOAT, 0, 0);
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, 0, (const GLvoid*)(m_verts.size() * sizeof(GLfloat)));
  
  glDrawArrays(drawMode, 0, m_verts.size() / 4);
}













