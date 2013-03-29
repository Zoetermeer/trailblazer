#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.hpp"

enum class PixmapFormat {
  TGA,
  BMP
};

class Texture {
private:
  const char *m_fileName;
  PixmapFormat m_format;
  GLuint m_textureId;
  
  GLenum m_minFilter;
  GLenum m_magFilter;
  GLenum m_wrapMode;
  
public:
  Texture(const char *fileName, PixmapFormat format, GLenum minFilter, GLenum magFilter, GLenum wrapMode);
  ~Texture();
  
public:
  const char *getFileName() { return m_fileName; }
  GLuint getTextureId() const { return m_textureId; }
  PixmapFormat getFormat() const { return m_format; }
  
private:
  void loadTGATexture(GLenum minFilter, GLenum magFilter, GLenum wrapMode);
  
public:
  void load();
  void use();
};

class TextureSet {
public:
  TextureSet();
  ~TextureSet();
  
public:
  void add(Texture *texture);
};

#endif