#ifndef MATRIX_H
#define MATRIX_H

#include "common.hpp"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class MatrixStack {
private:
  std::vector<glm::mat4> m_stack;
  glm::mat4 m_cur;
  
public:
  MatrixStack()
  {
    
  }
  
  //Assignable
  glm::mat4 &current()
  {
    return m_cur;
  }
  
  //Returns a copy
  glm::mat4 getCurrent() const { return m_cur; }
  
  void syncToGlStack(GLenum mode)
  {
    glMatrixMode(mode);
    glLoadMatrixf(glm::value_ptr(this->current()));
  }
  
  void syncToGlStack(GLenum mode, const glm::mat4 &mat)
  {
    glMatrixMode(mode);
    glLoadMatrixf(glm::value_ptr(mat));
  }
  
  void loadIdentity()
  {
    m_cur = glm::mat4();
  }
  
  void pushMatrix()
  {
    m_stack.push_back(m_cur);
  }
  
  void pushMatrix(const glm::mat4 mat)
  {
    m_stack.push_back(m_cur);
    m_cur = mat;
  }
  
  void popMatrix()
  {
    m_cur = m_stack.back();
    m_stack.pop_back();
  }
  
  glm::mat4 peek() const
  {
    return m_stack.back();
  }
  
  glm::mat3 getNormalMatrix() const
  {
    glm::mat3 nm = glm::mat3(m_cur);
    return glm::transpose(glm::inverse(nm));
  }
  
  void translate(GLfloat x, GLfloat y, GLfloat z)
  {
    m_cur = glm::translate(m_cur, glm::vec3(x, y, z));
  }
  
  void translateX(GLfloat x)
  {
    translate(x, 0.f, 0.f);
  }
  
  void translateY(GLfloat y)
  {
    translate(0.f, y, 0.f);
  }
  
  void translateZ(GLfloat z)
  {
    translate(0.f, 0.f, z);
  }
  
  void rotate(GLfloat deg, GLfloat x, GLfloat y, GLfloat z)
  {
    if (!x && !y && !z)
      return;
    
    m_cur = glm::rotate(m_cur, deg, glm::vec3(x, y, z));
  }
  
  void rotateX(GLfloat deg)
  {
    rotate(deg, 1.f, 0.f, 0.f);
  }
  
  void rotateY(GLfloat deg)
  {
    rotate(deg, 0.f, 1.f, 0.f);
  }
  
  void rotateZ(GLfloat deg)
  {
    rotate(deg, 0.f, 0.f, 1.f);
  }
  
  void scale(GLfloat x, GLfloat y, GLfloat z)
  {
    m_cur = glm::scale(m_cur, glm::vec3(x, y, z));
  }
  
  void scaleX(GLfloat fac)
  {
    scale(fac, 1.f, 1.f);
  }
  
  void scaleY(GLfloat fac)
  {
    scale(1.f, fac, 1.f);
  }
  
  void scaleZ(GLfloat fac)
  {
    scale(1.f, 1.f, fac);
  }
  
  void perspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
  {
    m_cur = glm::perspective(fovy, aspect, zNear, zFar);
  }
};

#endif