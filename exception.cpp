#include "exception.hpp"
#include <string>
#include <sstream>

FileNotFoundException::FileNotFoundException(const char *fileName)
: std::exception(), m_fileName(fileName)
{
  
}

const char *FileNotFoundException::what() const throw()
{
  std::string wh = "File not found: ";
  wh += m_fileName;
  return wh.c_str();
}

ShaderCompileException::ShaderCompileException(const char *sourceFileName, const char *details)
: std::exception(), m_sourceFileName(sourceFileName), m_details(details)
{
  
}

const char *ShaderCompileException::what() const throw()
{
  std::string wh = m_sourceFileName;
  wh += ": ";
  wh += m_details;
  return wh.c_str();
}

OpenGLException::OpenGLException(const char *fileName, int lineNum, GLenum errorCode)
: std::exception(), m_fileName(fileName), m_lineNum(lineNum)
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

const char *OpenGLException::what() const throw()
{
  std::stringstream *str = new std::stringstream();
  *str << m_fileName << ": " << m_lineNum << ": " << m_details;
  return str->str().c_str();
}