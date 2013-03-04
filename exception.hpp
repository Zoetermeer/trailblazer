#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <OpenGL/gl.h>

class FileNotFoundException : public std::exception {
private:
  const char *m_fileName;
  
public:
  FileNotFoundException(const char *fileName);
  ~FileNotFoundException() throw() { }
  virtual const char* what() const throw();
};

class ShaderCompileException : public std::exception {
private:
  const char *m_sourceFileName;
  const char *m_details;
  
public:
  ShaderCompileException(const char *sourceFileName, const char *details);
  ~ShaderCompileException() throw() { }
  virtual const char *what() const throw();
};

class OpenGLException : public std::exception {
private:
  const char *m_fileName;
  int m_lineNum;
  const char *m_details;
  
public:
  OpenGLException(const char *fileName, int lineNum, GLenum errorCode);
  ~OpenGLException() throw() { }
  virtual const char *what() const throw();
};

#endif