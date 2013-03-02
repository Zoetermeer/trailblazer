#include <cmath>
#include "GL.hpp"
#include "exception.hpp"
#include "shader.hpp"
#include "vertex-batch.hpp"
#include "matrix-stack.hpp"
#include "env.hpp"

glm::vec4 GL::WHITE = glm::vec4(1.0, 1.0, 1.0, 1.0);
glm::vec4 GL::GREEN = glm::vec4(0.0, 1.0, 0.0, 1.0);
glm::vec4 GL::RED = glm::vec4(1.0, 0.0, 0.0, 1.0);
glm::vec4 GL::BLUE = glm::vec4(0.0, 0.0, 1.0, 1.0);
glm::vec4 GL::GRAY = glm::vec4(0.5, 0.5, 0.5, 1.0);
glm::vec4 GL::BLACK = glm::vec4(0.0, 0.0, 0.0, 1.0);
glm::vec4 GL::YELLOW = glm::vec4(1.0, 1.0, 0.0, 1.0);

void GL::drawText(double x, double y, const char *str)
{
  int i, imax;
  glRasterPos2d(x, y);
  imax = 1023;
  for (i = 0; str[i] != '\0' && i < imax; i++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);
  }
}

void GL::drawCircle(unsigned segments)
{/*
  GLfloat angle;
  glBegin(GL_LINE_LOOP);
  glPushMatrix();
  {
    //glScalef(radius, radius, 1.0);
    for (angle = 0.0; angle <= 360.0; angle += 360.0 / segments) {
      glVertex2d(cos(DEG_TO_RAD(angle)), sin(DEG_TO_RAD(angle)));
    }
  }
  glPopMatrix();
  glEnd();
  */
}

void GL::drawPlane() throw (OpenGLException*)
{
  GLfloat i;
  static VertexBatch batch;
  static bool bound = false;
  if (!bound) {
    //Same normal for all
    const glm::vec3 nrm = glm::vec3(0.0f, 1.0f, 0.0f);
    batch.begin();
    for (i = -999.0; i < 1000.0; i += 4) {
      batch.add(i, 0.0, -1000.0, nrm.x, nrm.y, nrm.z);
      batch.add(i, 0.0, 1000.0, nrm.x, nrm.y, nrm.z);
      batch.add(-1000.0, 0.0, i, nrm.x, nrm.y, nrm.z);
      batch.add(1000.0, 0.0, i, nrm.x, nrm.y, nrm.z);
    }
    
    batch.end();
    bound = true;
    CHECK_OPENGL_ERROR;
  }
  
  batch.draw(GL_LINES);
  CHECK_OPENGL_ERROR;
}

void GL::drawAxes(Env &env) throw (OpenGLException*)
{
  static VertexBatch batch;
  static bool bound = false;
  if (!bound) {
    //Same normal for all vertices here
    //Facing up?
    const glm::vec3 nrm = glm::vec3(0.0f, 1.0f, 0.0f);
    batch.begin();
    batch.add(-AXIS_LENGTH, 0.0, 0.0, nrm.x, nrm.y, nrm.z);
    batch.add(AXIS_LENGTH, 0.0, 0.0, nrm.x, nrm.y, nrm.z);
    batch.add(0.0, -AXIS_LENGTH, 0.0, nrm.x, nrm.y, nrm.z);
    batch.add(0.0, AXIS_LENGTH, 0.0, nrm.x, nrm.y, nrm.z);
    batch.add(0.0, 0.0, -AXIS_LENGTH, nrm.x, nrm.y, nrm.z);
    batch.add(0.0, 0.0, AXIS_LENGTH, nrm.x, nrm.y, nrm.z);
    batch.end();
    bound = true;
    CHECK_OPENGL_ERROR;
  }
  
  batch.draw(GL_LINES);
  CHECK_OPENGL_ERROR;
  
  MatrixStack &mv = env.getMV();
  ShaderSet &shaders = env.getShaders();
  
  //Label axes
  mv.pushMatrix();
  {
    mv.translateX(-200.f);
    for (int i = -10; i < 10; i++) {
      mv.pushMatrix();
      {
        mv.scale(0.f, 2.f, 0.f);
        shaders.current().uniform(Uniform::COLOR) = GL::RED;
        shaders.current().uniform(Uniform::MODELVIEW_MATRIX) = mv.current();
        GL::drawLine();
      }
      mv.popMatrix();
      mv.pushMatrix();
      {
        mv.translateY(2.f);
        shaders.current().uniform(Uniform::COLOR) = GL::WHITE;
        shaders.current().uniform(Uniform::MODELVIEW_MATRIX) = mv.current();
        char str[12];
        sprintf(str, "X = %d", 20 * i);
        GL::drawText(0.0, 0.0, str);
      }
      mv.popMatrix();
      mv.translateX(20.f);
    }
  }
  mv.popMatrix();
  
  mv.pushMatrix();
  {
    mv.translateZ(-200.f);
    for (int i = -10; i < 10; i++) {
      mv.pushMatrix();
      {
        mv.scale(0.f, 2.f, 0.f);
        shaders.current().uniform(Uniform::COLOR) = GL::RED;
        shaders.current().uniform(Uniform::MODELVIEW_MATRIX) = mv.current();
        GL::drawLine();
      }
      mv.popMatrix();
      mv.pushMatrix();
      {
        mv.translateY(2.f);
        shaders.current().uniform(Uniform::COLOR) = GL::WHITE;
        shaders.current().uniform(Uniform::MODELVIEW_MATRIX) = mv.current();
        char str[12];
        sprintf(str, "Z = %d", 20 * i);
        GL::drawText(0.0, 0.0, str);
      }
      mv.popMatrix();
      mv.translateZ(20.f);
    }
  }
  mv.popMatrix();
}

void GL::drawSphere(unsigned stacks, unsigned slices) throw (OpenGLException*)
{/*
  static VertexBatch batch;
  static bool bound = false;
  if (!bound) {
    
  }
  
  batch.draw(GL_TRIANGLES);
  CHECK_OPENGL_ERROR;
  */
}

void GL::drawBox(GLenum mode) throw (OpenGLException*)
{
  static VertexBatch batch;
  static bool bound = false;
  if (!bound) {
    batch.begin();
    batch.add(-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f);
    batch.add(-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f);
    batch.add(-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f); //Facing left
    batch.add(1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f); //Top-right-back
    batch.add(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f); //Bottom-left-back
    batch.add(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f); //Top-left-back (facing back)
    batch.add(1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f); //Right-bottom-front
    batch.add(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f); //Left-bottom-back
    batch.add(1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f); //Right-bottom-back (facing bottom)
    batch.add(1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f); //Right-top-back
    batch.add(1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f); //Right-bottom-back
    batch.add(-1.0f,-1.0f,-1.0f, 0.0f, 0.0f, -1.0f); //Left-bottom-back (back facing)
    batch.add(-1.0f,-1.0f,-1.0f, -1.0f, 0.0f, 0.0f); //Left-bottom-back
    batch.add(-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f); //Left-top-front
    batch.add(-1.0f, 1.0f,-1.0f, -1.0f, 0.0f, 0.0f); //Left-top-back (facing left)
    batch.add(1.0f,-1.0f, 1.0f, 0.0f, -1.0f, 0.0f); //Right-bottom-front
    batch.add(-1.0f,-1.0f, 1.0f, 0.0f, -1.0f, 0.0f); //Left-bottom-front
    batch.add(-1.0f,-1.0f,-1.0f, 0.0f, -1.0f, 0.0f); //Left-bottom-back (bottom facing)
    batch.add(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f); //Left-top-front
    batch.add(-1.0f,-1.0f, 1.0f, 0.0f, 0.0f, 1.0f); //Left-bottom-front
    batch.add(1.0f,-1.0f, 1.0f, 0.0f, 0.0f, 1.0f); //Right-bottom-front (facing front)
    batch.add(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f); //Right-top-front
    batch.add(1.0f,-1.0f,-1.0f, 1.0f, 0.0f, 0.0f); //Right-bottom-back
    batch.add(1.0f, 1.0f,-1.0f, 1.0f, 0.0f, 0.0f); //Right-top-back (right-facing)
    batch.add(1.0f,-1.0f,-1.0f, 1.0f, 0.0f, 0.0f); //Right-bottom-back
    batch.add(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f); //Right-top-front
    batch.add(1.0f,-1.0f, 1.0f, 1.0f, 0.0f, 0.0f); //Right-bottom-front (right-facing)
    batch.add(1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f); //Right-top-front
    batch.add(1.0f, 1.0f,-1.0f, 0.0f, 1.0f, 0.0f); //Right-top-back
    batch.add(-1.0f, 1.0f,-1.0f, 0.0f, 1.0f, 0.0f); //Left-top-back (top-facing)
    batch.add(1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f); //Right-top-front
    batch.add(-1.0f, 1.0f,-1.0f, 0.0f, 1.0f, 0.0f); //Left-top-back
    batch.add(-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f); //Left-top-front (top-facing)
    batch.add(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f); //Right-top-front
    batch.add(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f); //Left-top-front
    batch.add(1.0f,-1.0f, 1.0f, 0.0f, 0.0f, 1.0f); //Right-bottom-front (front-facing)
    
    batch.end();
    bound = true;
    CHECK_OPENGL_ERROR;
  }
  
  batch.draw(GL_TRIANGLES);
  CHECK_OPENGL_ERROR;
}

void GL::drawFilledWireframeBox(const vec3_t<GLfloat> &lineColor, const vec3_t<GLfloat> &faceColor)
{
  glColor3f(faceColor.x, faceColor.y, faceColor.z);
  GL::drawBox(GL_TRIANGLES);
  glLineWidth(3.0);
  glColor3f(lineColor.x, lineColor.y, lineColor.z);
  GL::drawBox(GL_LINE_LOOP);
  glLineWidth(1.0);
}

void GL::drawLine() throw (OpenGLException*)
{
  static VertexBatch batch;
  static bool bound;
  if (!bound) {
    //Usually drawing lines without a shader,
    //but if using a shader what should the normals be?
    const glm::vec3 nrm = glm::vec3(0.f, 0.f, 0.f);
    batch.begin();
    batch.add(0.f, 0.f, 0.f, nrm.x, nrm.y, nrm.z);
    batch.add(1.f, 1.f, 1.f, nrm.x, nrm.y, nrm.z);
    batch.end();
    bound = true;
    CHECK_OPENGL_ERROR;
  }
  
  batch.draw(GL_LINES);
  CHECK_OPENGL_ERROR;
}

void GL::drawBackgroundStars() throw (OpenGLException*)
{
  static VertexBatch batch;
  static bool bound;
  if (!bound) {
    //We don't care about normals here -- don't use
    //shader to draw background stars
    const glm::vec3 nrm = glm::vec3(0.0f, 0.0f, 0.0f);
    const GLfloat radius = 8000.0f;
    batch.begin();
    for (unsigned i = 0; i < 50000; i++) {
      GLfloat azimuth = DEG_TO_RAD(randAngle());
      GLfloat polar = DEG_TO_RAD(randAngle());
      batch.add(radius * sin(azimuth) * cos(polar),
                radius * sin(azimuth) * sin(polar),
                radius * cos(azimuth),
                nrm.x, nrm.y, nrm.z);
    }
    
    batch.end();
    bound = true;
    CHECK_OPENGL_ERROR;
  }
  
  batch.draw(GL_POINTS);
  CHECK_OPENGL_ERROR;
}

#define RECUR(x, z, b, ms) \
ms.pushMatrix(); \
{\
ms.translate(x, 0.f, z); \
ms.scale(.5f, 1.f, .5f); \
surfaceGeometry(step - 1, b, ms); \
}\
ms.popMatrix();
void GL::surfaceGeometry(unsigned step, VertexBatch *batch, MatrixStack &tform)
{
  if (!step) {
    addBox(batch, tform, true, false);
    return;
  }
  
  if (percentChance(50.f)) {
    //Add a box, then translate to its top surface
    addBox(batch, tform, false, false);
    tform.translateY(1.f);
  }
  
  //Subdivide into four quadrants, generate smaller boxes
  RECUR(-.5f, -.5f, batch, tform);
  RECUR(.5f, -.5f, batch, tform);
  RECUR(-.5f, .5f, batch, tform);
  RECUR(.5f, .5f, batch, tform);
}

#define SURFACE_GEOM(rot, scale) \
{\
  stack.pushMatrix(); \
  {\
    rot; \
    if (!allowExtension) {\
      stack.translateY(1.f); \
      scale; \
    } else \
      stack.translateY(1.f); \
    surfaceGeometry(steps, b, stack); \
  }\
  stack.popMatrix(); \
}
void GL::generateBoxWithSurfaceNoise(VertexBatch *b, unsigned steps, bool allowExtension)
{
  MatrixStack stack;
  b->begin();
  addBox(b, stack, false, false);
  stack.pushMatrix();
  {
    if (!allowExtension) {
      stack.translateY(1.f);
      stack.scale(1.f, .1f, 1.f);
    } else 
      stack.translateY(2.f);
    surfaceGeometry(steps, b, stack);
  }
  stack.popMatrix();
  stack.pushMatrix();
  {
    stack.rotateX(180.f);
    if (!allowExtension) {
      stack.translateY(1.f);
      stack.scale(1.f, .1f, 1.f);
    } else
      stack.translateY(2.f);
    surfaceGeometry(steps, b, stack);
  }
  stack.popMatrix();
  stack.pushMatrix();
  {
    stack.rotateZ(90.f);
    stack.translateY(1.f);
    stack.scale(allowExtension ? 4.f : 1.f, .1f, 1.f);
    surfaceGeometry(steps, b, stack);
  }
  stack.popMatrix();
  stack.pushMatrix();
  {
    stack.rotateZ(-90.f);
    stack.translateY(1.f);
    stack.scale(allowExtension ? 4.f : 1.f, .1f, 1.f);
    surfaceGeometry(steps, b, stack);
  }
  stack.popMatrix();
  stack.pushMatrix();
  {
    stack.rotateX(90.f);
    stack.translateY(1.f);
    stack.scale(1.f, .1f, allowExtension ? 4.f : 1.f);
    surfaceGeometry(steps, b, stack);
  }
  stack.popMatrix();
  stack.pushMatrix();
  {
    stack.rotateX(-90.f);
    stack.translateY(1.f);
    stack.scale(1.f, .1f, allowExtension ? 4.f : 1.f);
    surfaceGeometry(steps, b, stack);
  }
  stack.popMatrix();
  
  b->end();
}

#define VERT(x,y,z) (m * glm::vec4(x,y,z,1.f))
#define NORMAL(a,b,c) (glm::normalize(glm::cross(glm::vec3(b - a), glm::vec3(c - a))))
#define ADD(x1,y1,z1,x2,y2,z2,x3,y3,z3) \
{\
  glm::vec4 v1 = VERT(x1,y1,z1); \
  glm::vec4 v2 = VERT(x2,y2,z2); \
  glm::vec4 v3 = VERT(x3,y3,z3); \
  glm::vec3 nrm = NORMAL(v1,v2,v3); \
  batch->add(v1, nrm);\
  batch->add(v2, nrm); \
  batch->add(v3, nrm); \
}
void GL::addBox(VertexBatch *batch, MatrixStack &stack, bool includeTop, bool includeBottom)
{
  glm::mat4 &m = stack.current();
  ADD(-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f);
  ADD(1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f);
  if (includeBottom) {
    ADD(1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f);
  }
  //batch->add(m * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), m * glm::vec4(-1.0f, 0.0f, 0.0f, 1.f));
  //batch->add(m * glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), m * glm::vec4(-1.0f, 0.0f, 0.0f, 1.f));
  //batch->add(m * glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), m * glm::vec4(-1.0f, 0.0f, 0.0f, 1.f)); //Facing left
  //batch->add(m * glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), m * glm::vec4(0.0f, 0.0f, -1.0f, 1.f)); //Top-right-back
  //batch->add(m * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), m * glm::vec4(0.0f, 0.0f, -1.0f, 1.f)); //Bottom-left-back
  //batch->add(m * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), m * glm::vec4(0.0f, 0.0f, -1.0f, 1.f)); //Top-left-back (facing back)
  if (includeBottom) {
    //batch->add(m * glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), m * glm::vec4(0.0f, -1.0f, 0.0f, 1.f)); //Right-bottom-front
    //batch->add(m * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), m * glm::vec4(0.0f, -1.0f, 0.0f, 1.f)); //Left-bottom-back
    //batch->add(m * glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), m * glm::vec4(0.0f, -1.0f, 0.0f, 1.f)); //Right-bottom-back (facing bottom)
  }
  
  ADD(1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f);
  ADD(-1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f);
  if (includeBottom) {
    ADD(1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f);
  }
  //batch->add(m * glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), m * glm::vec4(0.0f, 0.0f, -1.0f, 1.f)); //Right-top-back
  //batch->add(m * glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), m * glm::vec4(0.0f, 0.0f, -1.0f, 1.f)); //Right-bottom-back
  //batch->add(m * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), m * glm::vec4(0.0f, 0.0f, -1.0f, 1.f)); //Left-bottom-back (back facing)
  //batch->add(m * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), m * glm::vec4(-1.0f, 0.0f, 0.0f, 1.f)); //Left-bottom-back
  //batch->add(m * glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), m * glm::vec4(-1.0f, 0.0f, 0.0f, 1.f)); //Left-top-front
  //batch->add(m * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), m * glm::vec4(-1.0f, 0.0f, 0.0f, 1.f)); //Left-top-back (facing left)
  if (includeBottom) {
    //batch->add(m * glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), m * glm::vec4(0.0f, -1.0f, 0.0f, 1.f)); //Right-bottom-front
    //batch->add(m * glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), m * glm::vec4(0.0f, -1.0f, 0.0f, 1.f)); //Left-bottom-front
    //batch->add(m * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), m * glm::vec4(0.0f, -1.0f, 0.0f, 1.f)); //Left-bottom-back (bottom facing)
  }
  
  ADD(-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f);
  ADD(1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f);
  ADD(1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f);
  if (includeTop) {
    ADD(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f);
    ADD(1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f);
  }
  
  ADD(1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f);
  //batch->add(m * glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), m * glm::vec4(0.0f, 0.0f, 1.0f, 1.f)); //Left-top-front
  //batch->add(m * glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), m * glm::vec4(0.0f, 0.0f, 1.0f, 1.f)); //Left-bottom-front
  //batch->add(m * glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), m * glm::vec4(0.0f, 0.0f, 1.0f, 1.f)); //Right-bottom-front (facing front)
  //batch->add(m * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), m * glm::vec4(1.0f, 0.0f, 0.0f, 1.f)); //Right-top-front
  //batch->add(m * glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), m * glm::vec4(1.0f, 0.0f, 0.0f, 1.f)); //Right-bottom-back
  //batch->add(m * glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), m * glm::vec4(1.0f, 0.0f, 0.0f, 1.f)); //Right-top-back (right-facing)
  //batch->add(m * glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), m * glm::vec4(1.0f, 0.0f, 0.0f, 1.f)); //Right-bottom-back
  //batch->add(m * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), m * glm::vec4(1.0f, 0.0f, 0.0f, 1.f)); //Right-top-front
  //batch->add(m * glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), m * glm::vec4(1.0f, 0.0f, 0.0f, 1.f)); //Right-bottom-front (right-facing)
  if (includeTop) {
    //batch->add(m * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), m * glm::vec4(0.0f, 1.0f, 0.0f, 1.f)); //Right-top-front
    //batch->add(m * glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), m * glm::vec4(0.0f, 1.0f, 0.0f, 1.f)); //Right-top-back
    //batch->add(m * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), m * glm::vec4(0.0f, 1.0f, 0.0f, 1.f)); //Left-top-back (top-facing)
    //batch->add(m * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), m * glm::vec4(0.0f, 1.0f, 0.0f, 1.f)); //Right-top-front
    //batch->add(m * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), m * glm::vec4(0.0f, 1.0f, 0.0f, 1.f)); //Left-top-back
    //batch->add(m * glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), m * glm::vec4(0.0f, 1.0f, 0.0f, 1.f)); //Left-top-front (top-facing)
  }
  //batch->add(m * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), m * glm::vec4(0.0f, 0.0f, 1.0f, 1.f)); //Right-top-front
  //batch->add(m * glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), m * glm::vec4(0.0f, 0.0f, 1.0f, 1.f)); //Left-top-front
  //batch->add(m * glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), m * glm::vec4(0.0f, 0.0f, 1.0f, 1.f)); //Right-bottom-front (front-facing)
}



















