#ifndef VERTEX_H
#define VERTEX_H

typedef struct vertex {
  GLuint index;
  glm::vec4 position;
  glm::vec3 normal;
  glm::vec4 color;
  GLfloat ao_accessibility;
  glm::ivec3 voxel_coordinate;
} vertex_t;

typedef struct vertex_spec {
  bool indexed;
  GLuint next_index;
  bool use_color;
  bool use_ao;
  bool use_voxel_coordinates;
} vertex_spec_t;

inline vertex_t new_vertex()
{
  vertex_t v;
  memset(&v, 0, sizeof(vertex_t));
  v.position.w = 1.f;
  return v;
}

inline vertex_t new_vertex(const glm::vec4 &pos)
{
  vertex_t v = new_vertex();
  v.position = pos;
  return v;
}

inline vertex_t new_vertex(GLfloat x, GLfloat y, GLfloat z)
{
  return new_vertex(glm::vec4(x, y, z, 1.f));
}

inline vertex_t new_vertex(GLfloat x, GLfloat y, GLfloat z, const glm::mat4 &m)
{
  return new_vertex(m * glm::vec4(x, y, z, 1.f));
}

inline glm::vec3 calc_normal(const vertex_t &a, const vertex_t &b, const vertex_t &c)
{
  return glm::normalize(glm::cross(glm::vec3(b.position - a.position), glm::vec3(c.position - a.position)));
}

#endif