uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;

uniform vec3 LightPosition;
uniform vec3 HeadlightPosition;
uniform vec3 HeadlightDir;

uniform bool Animating;
uniform float Time;

attribute float AOAccessibility;
attribute vec4 Color;
attribute vec3 VoxelCoordinate;

varying vec3 VaryingNormal;
varying vec3 VaryingLightDir;
varying vec3 VaryingHeadlightDir;
varying float VaryingAccessibility;
varying vec4 VertexColor;

void main()
{
  vec4 eyePos = vec4(ModelViewMatrix * gl_Vertex);
  VaryingNormal = NormalMatrix * gl_Normal;
  VaryingLightDir = LightPosition - eyePos.xyz;
  //VaryingHeadlightDir = normalize(HeadlightDir);
  VaryingHeadlightDir = normalize(HeadlightPosition - eyePos.xyz);
  VaryingAccessibility = AOAccessibility;
  VertexColor = Color;
  gl_Position = ProjectionMatrix * eyePos;
}