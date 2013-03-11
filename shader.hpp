#ifndef SHADER_H
#define SHADER_H

#include <vector>
#include <string>
#include "exception.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GL.hpp"

enum class ShaderType {
  Default,
  Diffuse,
  Gouraud,
  Phong,
  Brick,
  Ship,
  Distortion,
  Hemisphere,
  HemisphereAmbientOcclusion
};

enum class VertexAttrib {
  None,
  Color, 
  AOAccessibility
};

class Uniform {
private:
  const char *m_name;
  GLuint m_loc;
  bool m_locAssigned;
  GLuint m_progId;
  
public:
  Uniform(const char *name, GLuint progId)
  : m_name(name), m_locAssigned(false), m_loc(0), m_progId(progId)
  {
    findLocation();
  }
  
public:
  //Well-known names of common uniforms
  static const char *MODELVIEW_MATRIX;
  static const char *PROJECTION_MATRIX;
  static const char *NORMAL_MATRIX;
  static const char *COLOR;
  static const char *AMBIENT_COLOR;
  static const char *DIFFUSE_COLOR;
  static const char *SPECULAR_COLOR;
  
public:
  const char *getName() const { return m_name; }
  void setLocation(GLuint loc) { m_loc = loc; }
  bool hasLocationAssigned() const { return m_locAssigned; }
  
  void findLocation()
  {
    if (!m_locAssigned) {
      m_loc = glGetUniformLocation(m_progId, m_name);
      m_locAssigned = true;
    }
  }
  
  inline void operator=(const glm::mat4 &m)
  {
    glUniformMatrix4fv(m_loc, 1, GL_FALSE, glm::value_ptr(m));
  }
  
  inline void operator=(const glm::mat3 &m)
  {
    glUniformMatrix3fv(m_loc, 1, GL_FALSE, glm::value_ptr(m));
  }
  
  inline void operator=(const glm::vec3 &v)
  {
    glUniform3fv(m_loc, 1, glm::value_ptr(v));
  }
  
  inline void operator=(const glm::vec4 &v)
  {
    glUniform4fv(m_loc, 1, glm::value_ptr(v));
  }
  
  inline void operator=(GLfloat *glmat)
  {
    glUniformMatrix4fv(m_loc, 1, GL_FALSE, glmat);
  }
};

class Shader {
private:
  std::string m_code;
  GLenum m_shaderType;
  GLuint m_shaderObjId;
  const char *m_sourceFileName;
  bool m_compiled;
  
private:
  Shader(const Shader &other) { }
  
public:
  Shader(const char *file, GLenum shaderType) throw (FileNotFoundException)
  : m_sourceFileName(file), m_shaderType(shaderType), m_compiled(false)
  {
    
  }
  
  ~Shader()
  {
    glDeleteShader(m_shaderObjId);
  }
  
public:
  const char *getSourceFileName() const { return m_sourceFileName; }
  const char *getCode() const { return m_code.c_str(); }
  GLuint getShaderObjId() const { return m_shaderObjId; }
  void setShaderObjId(GLuint id) { m_shaderObjId = id; }
  bool isCompiled() const { return m_compiled; }
  
private:
  std::string readSource();
  
public:
  void load();
  void compile();
};

class ShaderProgram {
private:
  ShaderType m_type;
  Shader *m_vertexShader;
  Shader *m_fragShader;
  GLuint m_id;
  bool m_enabled;
  std::vector<Uniform> m_uniforms;
  
public:
  ShaderProgram(ShaderType type,
                const char *vShader,
                const char *fShader)
  : m_type(type), m_enabled(false)
  {
    m_vertexShader = new Shader(vShader, GL_VERTEX_SHADER);
    m_fragShader = new Shader(fShader, GL_FRAGMENT_SHADER);
  }
  
  ~ShaderProgram()
  {
    delete m_vertexShader;
    delete m_fragShader;
    glDeleteProgram(m_id);
  }
  
  ShaderType getType() const { return m_type; }
  const GLuint getId() const { return m_id; }
  Uniform &uniform(const char *name);
  
  void build();
  void enable();
  void disable();
  
  void prepare();
  
protected:
  virtual void bindAttributes();
};

class HemisphereAOShaderProgram : public ShaderProgram {
public:
  HemisphereAOShaderProgram()
  : ShaderProgram(ShaderType::HemisphereAmbientOcclusion, "shaders/hem-ao.vert", "shaders/default.frag")
  {
    
  }

protected:
  virtual void bindAttributes();
};

class ShaderSet {
private:
  std::vector<Shader> m_vertShaders;
  std::vector<Shader> m_fragShaders;
  std::vector<ShaderProgram*> m_programs;
  ShaderProgram *m_cur;
  
public:
  ShaderSet()
  : m_cur(NULL)
  {
    
  }
  
  ~ShaderSet()
  {
    for (ShaderProgram *p : m_programs)
      delete p;
  }
  
private:
  bool tryFindShader(GLenum type, const char *fileName, GLuint *id);
  
public:
  ShaderProgram &current();
  ShaderProgram &use(ShaderType type) throw (FileNotFoundException*);
  void disable();
  void add(ShaderProgram *prog) throw (ShaderCompileException*, OpenGLException*, FileNotFoundException*);
  void add(ShaderType type, 
           const char *vertShader,
           const char *fragShader) throw (ShaderCompileException*, OpenGLException*, FileNotFoundException*);
  void prepareDefault(Env &env, const glm::vec4 &color);
  void preparePhong(Env &env, const glm::vec4 &amb, const glm::vec4 &diff, const glm::vec4 &spec);
  void prepareDistortion(Env &env, const glm::vec4 &amb, const glm::vec4 &diff, const glm::vec4 &spec);
  void prepareShip(Env &env, const glm::vec4 &amb, const glm::vec4 &diff, const glm::vec4 &spec);
  void prepareHemisphere(Env &env, const glm::vec3 &lightPos, const glm::vec4 &skyColor, const glm::vec4 &groundColor);
  void prepareHemisphereAO(Env &env, const glm::vec3 &lightPos, const glm::vec4 &skyColor, const glm::vec4 &groundColor);
};

#endif









