#include "lighting.hpp"

void DefaultAmbientLighting::init()
{
  enable();
}

void DefaultAmbientLighting::enable()
{
  //GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
  //GLfloat mat_shininess[] = { 100.0 };
  GLfloat light_position[] = { 0.0, 0.0, 0.0, 0.0 };
  //glClearColor(0.0, 0.0, 0.0, 0.0);
  //glShadeModel(GL_SMOOTH);
  
  //glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  //glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void DefaultAmbientLighting::disable()
{
  glDisable(GL_LIGHTING);
}