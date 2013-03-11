uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;

uniform vec3 LightPosition;

varying vec3 VaryingNormal;
varying vec3 VaryingLightDir;

void main()
{
  VaryingNormal = NormalMatrix * gl_Normal;
  
  vec4 pos4 = ModelViewMatrix * gl_Vertex;
  vec3 pos3 = pos4.xyz / pos4.w;
  
  //Vector to light source
  VaryingLightDir = normalize(LightPosition - pos3);
  
  gl_Position = ProjectionMatrix * (ModelViewMatrix * gl_Vertex);
}