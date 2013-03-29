#include "texture.hpp"
#include "images.hpp"
#include "GL.hpp"

Texture::Texture(const char *fileName, PixmapFormat format, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
: m_fileName(fileName),
  m_format(format),
  m_textureId(0),
  m_minFilter(minFilter),
  m_magFilter(magFilter),
  m_wrapMode(wrapMode)
{
  
}

Texture::~Texture()
{
  if (m_textureId)
    glDeleteTextures(1, &m_textureId);
}

void Texture::loadTGATexture(GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
  GLbyte *bits;
  int width, height, components;
  GLenum format;
  
  //May throw a file not found exception
  bits = Images::readTGA(m_fileName, &width, &height, &components, &format);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
  
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, components, width, height, 0, format, GL_UNSIGNED_BYTE, bits);
  
  free(bits);
  
  //Mipmap if specified
  switch (minFilter)
  {
    case GL_LINEAR_MIPMAP_LINEAR:
    case GL_LINEAR_MIPMAP_NEAREST:
    case GL_NEAREST_MIPMAP_LINEAR:
    case GL_NEAREST_MIPMAP_NEAREST:
      glGenerateMipmap(GL_TEXTURE_2D);
      break;
  }
}

void Texture::load()
{
  glGenTextures(1, &m_textureId);
  glBindTexture(GL_TEXTURE_2D, m_textureId);
  switch (m_format)
  {
    case PixmapFormat::TGA:
      loadTGATexture(m_minFilter, m_magFilter, m_wrapMode);
      break;
    case PixmapFormat::BMP:
      break;
  }
  
  CHECK_OPENGL_ERROR;
}

void Texture::use()
{
  if (m_textureId)
    glBindTexture(GL_TEXTURE_2D, m_textureId);
}


TextureSet::TextureSet()
{
  
}

TextureSet::~TextureSet()
{
  for (std::map<TextureType, Texture*>::iterator it = m_textures.begin(); it != m_textures.end(); ++it) {
    delete it->second;
  }
}

void TextureSet::add(TextureType type, PixmapFormat format, const char *fileName)
{
  Texture *t = new Texture(fileName, format, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
  t->load();
  m_textures[type] = t;
}

void TextureSet::use(TextureType type)
{
  Texture *t = m_textures[type];
  t->use();
}















