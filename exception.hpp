#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <string>
#include <sstream>
#include <OpenGL/gl.h>

class FileNotFoundException : public std::exception {
private:
  const char *m_fileName;
  
public:
  FileNotFoundException(const char *fileName)
  : exception(), m_fileName(fileName)
  {
    
  }
  
  const char* what() const throw()
  {
    return m_fileName;
  }
};

class ShaderCompileException : public std::exception {
private:
  const char *m_sourceFileName;
  const char *m_details;
  
public:
  ShaderCompileException(const char *sourceFileName, const char *details)
  : exception(), m_sourceFileName(sourceFileName), m_details(details)
  {
    
  }
  
  const char *what() const throw()
  {
    std::string wh = m_sourceFileName;
    wh += ": ";
    wh += m_details;
    return wh.c_str();
  }
};

class OpenGLException : public std::exception {
private:
  const char *m_fileName;
  int m_lineNum;
  const char *m_details;
  
public:
  OpenGLException(const char *fileName, int lineNum, GLenum errorCode)
  : exception(), m_fileName(fileName), m_lineNum(lineNum)
  {
    switch (errorCode)
    {
      case GL_NO_ERROR:
        m_details = "NO ERROR";
        break;
      case GL_INVALID_ENUM:
        m_details = "INVALID ENUM";
        break;
      case GL_INVALID_VALUE:
        m_details = "INVALID VALUE";
        break;
      case GL_INVALID_OPERATION:
        m_details = "INVALID OPERATION";
        break;
      case GL_STACK_OVERFLOW:
        m_details = "STACK OVERFLOW";
        break;
      case GL_STACK_UNDERFLOW:
        m_details = "STACK UNDERFLOW";
        break;
      case GL_OUT_OF_MEMORY:
        m_details = "OUT OF MEMORY";
        break;
      default:
        m_details = "(Unknown error code)";
        break;
    }
  }
  
  const char *what() const throw()
  {
    std::stringstream str;
    str << m_fileName << ": " << m_lineNum << ": " << m_details;
    return str.str().c_str();
  }
};

#endif