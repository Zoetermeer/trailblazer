#ifndef IMAGES_H
#define IMAGES_H

#include "common.hpp"

class Images {
public:
  static GLbyte *readTGA(const char *fileName, int *width, int *height, int *components, GLenum *format);
};

#endif