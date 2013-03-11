uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;

uniform vec3 LightPosition;

attribute float AOAccessibility;
attribute vec4 Color;

varying vec3 VaryingNormal;
varying vec3 VaryingLightDir;
varying float VaryingAccessibility;
varying vec4 VertexColor;

void main()
{
  vec4 eyePos = vec4(ModelViewMatrix * gl_Vertex);
  VaryingNormal = normalize(NormalMatrix * gl_Normal);
  VaryingLightDir = normalize(LightPosition - eyePos.xyz);
  VaryingAccessibility = AOAccessibility;
  VertexColor = Color;
  gl_Position = ProjectionMatrix * eyePos;
}