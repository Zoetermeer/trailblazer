#include <iostream>
#include <fstream>
#include "shader.hpp"
#include "matrix-stack.hpp"
#include "env.hpp"

const char *Uniform::MODELVIEW_MATRIX = "ModelViewMatrix";
const char *Uniform::NORMAL_MATRIX = "NormalMatrix";
const char *Uniform::PROJECTION_MATRIX = "ProjectionMatrix";
const char *Uniform::COLOR = "Color";
const char *Uniform::AMBIENT_COLOR = "AmbientColor";
const char *Uniform::DIFFUSE_COLOR = "DiffuseColor";
const char *Uniform::SPECULAR_COLOR = "SpecularColor";

std::string Shader::readSource()
{
  std::fstream inFile;
  std::string lineBuf;
  std::string ret;
  
  inFile.open(m_sourceFileName, std::ifstream::in);
  if (!inFile) {
    throw new FileNotFoundException(m_sourceFileName);
  }
  
  while (!inFile.eof()) {
    getline(inFile, lineBuf);
    lineBuf += '\n';
    ret += lineBuf;
  }
  
  inFile.close();
  return ret;
}

void Shader::load()
{
  m_code = readSource();
  const char *src = m_code.c_str();
  m_shaderObjId = glCreateShader(m_shaderType);
  glShaderSource(m_shaderObjId, 1, &src, NULL);
}

void Shader::compile()
{
  GLint compiled;
  glCompileShader(m_shaderObjId);
  glGetShaderiv(m_shaderObjId, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    //Try to get detailed info
    GLint blen;
    GLsizei slen;
    GLchar *compilerLog = NULL;
    glGetShaderiv(m_shaderObjId, GL_INFO_LOG_LENGTH, &blen);
    if (blen) {
      compilerLog = (GLchar*)malloc(blen * sizeof(GLchar));
      glGetShaderInfoLog(m_shaderObjId, blen, &slen, compilerLog);
    }
    
    throw new ShaderCompileException(m_sourceFileName, compilerLog);
    if (blen) free(compilerLog);
    return;
  }
  
  m_compiled = true;
}

Uniform &ShaderProgram::uniform(const char *name)
{
  for (unsigned i = 0; i < m_uniforms.size(); i++) {
    Uniform &u = m_uniforms.at(i);
    if (!strcmp(u.getName(), name))
      return u;
  }
  
  //Otherwise, create a new one?
  Uniform u(name, m_id);
  m_uniforms.push_back(u);
  return m_uniforms.back();
}

void ShaderProgram::bindAttributes()
{
  
}

void ShaderProgram::build()
{
  m_id = glCreateProgram();
  
  m_vertexShader->load();
  m_vertexShader->compile();
  m_fragShader->load();
  m_fragShader->compile();
  
  glAttachShader(m_id, m_vertexShader->getShaderObjId());
  glAttachShader(m_id, m_fragShader->getShaderObjId());
  
  bindAttributes();
  
  GLint linked;
  glLinkProgram(m_id);
  glGetProgramiv(m_id, GL_LINK_STATUS, &linked);
  if (linked == GL_FALSE) {
    //Try to get detailed info
    GLint blen;
    GLsizei slen;
    GLchar *linkerLog = NULL;
    glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &blen);
    if (blen) {
      linkerLog = (GLchar*)malloc(blen * sizeof(GLchar));
      glGetProgramInfoLog(m_id, blen, &slen, linkerLog);
    }
    
    throw new ShaderCompileException("LINKER", linkerLog);
    if (blen) free(linkerLog);
    return;
  }
  
  CHECK_OPENGL_ERROR;
}

void ShaderProgram::prepare()
{
  for (Uniform uni : m_uniforms) {
    uni.findLocation();
  }
}

void ShaderProgram::enable()
{
  if (!m_enabled) {
    glUseProgram(m_id);
    m_enabled = true;
  }
}

void ShaderProgram::disable()
{
  if (m_enabled) {
    glUseProgram(0);
    m_enabled = false;
  }
}

bool ShaderSet::tryFindShader(GLenum type, const char *fileName, GLuint *id)
{
  for (Shader &sh : type == GL_VERTEX_SHADER ? m_vertShaders : m_fragShaders) {
    if (!strcmp(fileName, sh.getSourceFileName())) {
      *id = sh.getShaderObjId();
      return true;
    }
  }
  
  return false;
}

void HemisphereAOShaderProgram::bindAttributes()
{
  glBindAttribLocation(getId(), (GLuint)VertexAttrib::AOAccessibility, "AOAccessibility");
  glBindAttribLocation(getId(), (GLuint)VertexAttrib::Color, "Color");
  glBindAttribLocation(getId(), (GLuint)VertexAttrib::VoxelCoordinate, "VoxelCoordinate");
}

void ShaderSet::add(ShaderType type, const char *vShader, const char *fShader)
  throw (ShaderCompileException*, OpenGLException*, FileNotFoundException*)
{
  auto *shprog = new ShaderProgram(type, vShader, fShader);
  add(shprog);
}

void ShaderSet::add(ShaderProgram *prog) throw (ShaderCompileException*, OpenGLException*, FileNotFoundException*)
{
  prog->build();
  CHECK_OPENGL_ERROR;
  
  m_programs.push_back(prog);
}

ShaderProgram &ShaderSet::use(ShaderType type) throw (FileNotFoundException*)
{
  if (m_cur && m_cur->getType() == type)
    return *m_cur;
  
  for (unsigned i = 0; i < m_programs.size(); i++) {
    auto prog = m_programs.at(i);
    if (type == prog->getType()) {
      if (m_cur)
        m_cur->disable();
      
      m_cur = prog;
      m_cur->enable();
      return *m_cur;
    }
  }
  
  throw new FileNotFoundException("Shader not found.");
}

ShaderProgram &ShaderSet::current()
{
  return *m_cur;
}

void ShaderSet::disable()
{
  if (m_cur) {
    m_cur->disable();
    m_cur = NULL;
  }
}

void ShaderSet::prepareDefault(Env &env, const glm::vec4 &color)
{
  ShaderProgram &sh = use(ShaderType::Default);
  sh.uniform(Uniform::MODELVIEW_MATRIX) = env.getMV().getCurrent();
  sh.uniform(Uniform::PROJECTION_MATRIX) = env.getProj().getCurrent();
  sh.uniform(Uniform::COLOR) = color;
}

void ShaderSet::preparePhong(Env &env,
                             const glm::vec4 &amb,
                             const glm::vec4 &diff,
                             const glm::vec4 &spec)
{
  ShaderProgram &sh = use(ShaderType::Phong);
  sh.uniform(Uniform::MODELVIEW_MATRIX) = env.getMV().getCurrent();
  sh.uniform(Uniform::PROJECTION_MATRIX) = env.getProj().getCurrent();
  sh.uniform(Uniform::NORMAL_MATRIX) = env.getMV().getNormalMatrix();
  sh.uniform("LightPosition") = glm::vec3(env.getSunPos());
  sh.uniform("HeadlightPosition") = env.getPlayer().getOffset();
  sh.uniform("HeadlightOn") = env.getPlayer().getIsHeadlightOn();
  sh.uniform("HeadlightDir") = env.getPlayer().getLookVector();
  sh.uniform(Uniform::AMBIENT_COLOR) = amb;
  sh.uniform(Uniform::DIFFUSE_COLOR) = diff;
  sh.uniform(Uniform::SPECULAR_COLOR) = spec;
}

void ShaderSet::prepareDistortion(Env &env, const glm::vec4 &amb, const glm::vec4 &diff, const glm::vec4 &spec)
{
  ShaderProgram &sh = use(ShaderType::Distortion);
  sh.uniform(Uniform::MODELVIEW_MATRIX) = env.getMV().getCurrent();
  sh.uniform(Uniform::PROJECTION_MATRIX) = env.getProj().getCurrent();
  sh.uniform(Uniform::NORMAL_MATRIX) = env.getMV().getNormalMatrix();
  sh.uniform(Uniform::AMBIENT_COLOR) = amb;
  sh.uniform(Uniform::DIFFUSE_COLOR) = diff;
  sh.uniform(Uniform::SPECULAR_COLOR) = spec;
}

void ShaderSet::prepareShip(Env &env, const glm::vec4 &amb, const glm::vec4 &diff, const glm::vec4 &spec)
{
  ShaderProgram &sh = use(ShaderType::Ship);
  sh.uniform(Uniform::MODELVIEW_MATRIX) = env.getMV().getCurrent();
  sh.uniform(Uniform::PROJECTION_MATRIX) = env.getProj().getCurrent();
  sh.uniform(Uniform::NORMAL_MATRIX) = env.getMV().getNormalMatrix();
  sh.uniform(Uniform::AMBIENT_COLOR) = amb;
  sh.uniform(Uniform::DIFFUSE_COLOR) = diff;
  sh.uniform(Uniform::SPECULAR_COLOR) = spec;
}

void ShaderSet::prepareHemisphere(Env &env, const glm::vec3 &lightPos, const glm::vec4 &skyColor, const glm::vec4 &groundColor)
{
  ShaderProgram &sh = use(ShaderType::Hemisphere);
  sh.uniform(Uniform::MODELVIEW_MATRIX) = env.getMV().getCurrent();
  sh.uniform(Uniform::PROJECTION_MATRIX) = env.getProj().getCurrent();
  sh.uniform(Uniform::NORMAL_MATRIX) = env.getMV().getNormalMatrix();
  sh.uniform("LightPosition") = lightPos;
  sh.uniform("SkyColor") = skyColor;
  sh.uniform("GroundColor") = groundColor;
}

void ShaderSet::prepareHemisphereAO(Env &env,
                                    const glm::vec3 &lightPos,
                                    const glm::vec3 &headlightPos,
                                    const glm::vec3 &headlightDir,
                                    const bool headlightOn,
                                    const glm::vec4 &skyColor,
                                    const glm::vec4 &groundColor,
                                    const bool animating,
                                    const GLfloat animationTime)
{
  ShaderProgram &sh = use(ShaderType::HemisphereAmbientOcclusion);
  sh.uniform(Uniform::MODELVIEW_MATRIX) = env.getMV().getCurrent();
  sh.uniform(Uniform::PROJECTION_MATRIX) = env.getProj().getCurrent();
  sh.uniform(Uniform::NORMAL_MATRIX) = env.getMV().getNormalMatrix();
  sh.uniform("LightPosition") = lightPos;
  sh.uniform("HeadlightPosition") = headlightPos;
  sh.uniform("HeadlightOn") = headlightOn;
  sh.uniform("HeadlightDir") = headlightDir;
  sh.uniform("SkyColor") = skyColor;
  sh.uniform("GroundColor") = groundColor;
  sh.uniform("Animating") = animating;
  sh.uniform("Time") = animationTime;
}











