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
    m_colors.push_back(vertex.color.x);
    m_colors.push_back(vertex.color.y);
    m_colors.push_back(vertex.color.z);
    m_colors.push_back(vertex.color.w);
  }
  
  if (m_vertexSpec.use_ao) {
    m_aoAttribs.push_back(vertex.ao_accessibility);
  }
  
  if (m_vertexSpec.use_voxel_coordinates) {
    m_vcAttribs.push_back(vertex.voxel_coordinate.x);
    m_vcAttribs.push_back(vertex.voxel_coordinate.y);
    m_vcAttribs.push_back(vertex.voxel_coordinate.z);
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
  m_vcAttribsSize = m_vcAttribs.size() * sizeof(GLuint);
  m_colorSize = m_colors.size() * sizeof(GLfloat);
  m_vertPtr = 0;
  m_normPtr = m_vertPtr + m_vertsSize;
  m_aoPtr = m_normPtr + m_normsSize;
  m_colPtr = m_aoPtr + m_aoAttribsSize;
  m_vcPtr = m_colPtr + m_colorSize;
  
  const size_t bufSize = m_vertsSize + m_normsSize + m_aoAttribsSize + m_colorSize + m_vcAttribsSize;
  glBufferData(GL_ARRAY_BUFFER, bufSize, NULL, GL_STATIC_DRAW);
  
  //Positions
  glBufferSubData(GL_ARRAY_BUFFER, m_vertPtr, m_vertsSize, verts);
  
  //Normals
  glBufferSubData(GL_ARRAY_BUFFER, m_normPtr, m_normsSize, norms);
  
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(4, GL_FLOAT, 0, (const GLvoid*)m_vertPtr);
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, 0, (const GLvoid*)m_normPtr);
  
  //Ambient occlusion (accessibility factor) vertex attribute
  //If we have vertex attributes, store them as a sub-buffer
  if (m_vertexSpec.use_ao) {
    GLfloat *accAtts = m_aoAttribs.data();
    glBufferSubData(GL_ARRAY_BUFFER, m_aoPtr, m_aoAttribsSize, accAtts);
    glEnableVertexAttribArray((GLuint)VertexAttrib::AOAccessibility);
    glVertexAttribPointer((GLuint)VertexAttrib::AOAccessibility,
                          1,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (const GLvoid*)m_aoPtr);
  }
  
  if (m_vertexSpec.use_color) {
    GLfloat *colors = m_colors.data();
    glBufferSubData(GL_ARRAY_BUFFER, m_colPtr, m_colorSize, colors);
    glEnableVertexAttribArray((GLuint)VertexAttrib::Color);
    glVertexAttribPointer((GLuint)VertexAttrib::Color,
                          4,
                          GL_FLOAT,
                          GL_TRUE,
                          0,
                          (const GLvoid*)m_colPtr);
  }
  
  if (m_vertexSpec.use_voxel_coordinates) {
    GLfloat *vcs = m_vcAttribs.data();
    glBufferSubData(GL_ARRAY_BUFFER, m_vcPtr, m_vcAttribsSize, vcs);
    glEnableVertexAttribArray((GLuint)VertexAttrib::VoxelCoordinate);
    glVertexAttribPointer((GLuint)VertexAttrib::VoxelCoordinate,
                          3,
                          GL_FLOAT,
                          GL_TRUE, 
                          0,
                          (const GLvoid*)m_vcPtr);
  }
  
  //Vertex element indices
  if (m_vertexSpec.indexed) {
    GLuint *inds = m_indices.data();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexSize, inds, GL_STATIC_DRAW);
  }
  
  CHECK_OPENGL_ERROR;
  glBindVertexArray(0);
  
  //Free up client-side data
  m_verts.clear();
  m_norms.clear();
  m_indices.clear();
  m_colors.clear();
  m_aoAttribs.clear();
  m_vcAttribs.clear();
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













