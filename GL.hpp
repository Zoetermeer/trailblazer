#ifndef GL_H
#define GL_H

#include <glm/glm.hpp>
#include "common.hpp"
#include "exception.hpp"

class Env;
class VertexBatch;
class MatrixStack;

#if (!defined(RELEASE))
#define CHECK_OPENGL_ERROR \
{ \
  GLenum err = glGetError(); \
  if (err != GL_NO_ERROR) \
    throw new OpenGLException(__FILE__, __LINE__, err); \
}
#else
#define CHECK_OPENGL_ERROR 
#endif

#define AXIS_LENGTH 500.f

class GL {
public:
  static glm::vec4 WHITE;
  static glm::vec4 GREEN;
  static glm::vec4 RED;
  static glm::vec4 BLUE;
  static glm::vec4 GRAY;
  static glm::vec4 BLACK;
  static glm::vec4 YELLOW;
  
  static inline void setColor(const glm::vec4 &c)
  {
    glColor4d(c.x, c.y, c.z, c.w);
  }
  
  static inline void rotateX(GLfloat degrees)
  {
    glRotatef(degrees, 1.0, 0.0, 0.0);
  }
  
  static inline void rotateY(GLfloat degrees)
  {
    glRotatef(degrees, 0.0, 1.0, 0.0);
  }
  
  static inline void rotateZ(GLfloat degrees)
  {
    glRotatef(degrees, 0.0, 0.0, 1.0);
  }
  
  static inline void translateX(GLfloat v)
  {
    glTranslatef(v, 0.f, 0.f);
  }
  
  static inline void translateY(GLfloat v)
  {
    glTranslatef(0.f, v, 0.f);
  }
  
  static inline void translateZ(GLfloat v)
  {
    glTranslatef(0.f, 0.f, v);
  }
  
  static void drawBox(GLenum mode) throw (OpenGLException*);
  static void drawFilledWireframeBox(const vec3_t<GLfloat> &lineColor, const vec3_t<GLfloat> &faceColor);
  static void drawText(double x, double y, const char *str);
  static void drawCircle(unsigned segments);
  static void drawSphere(unsigned stacks, unsigned slices) throw (OpenGLException*);
  static void drawPlane() throw (OpenGLException*);
  static void drawAxes(Env &env) throw (OpenGLException*);
  static void drawLine() throw (OpenGLException*);
  static void drawBackgroundStars() throw (OpenGLException*);
  static void generateBoxWithSurfaceNoise(VertexBatch *b, unsigned steps, bool allowExtension);
  static void addBox(VertexBatch *batch, MatrixStack &stack, bool includeTop, bool includeBottom);
  
private:
  static void surfaceGeometry(unsigned step, VertexBatch *batch, MatrixStack &tform);
};

#endif