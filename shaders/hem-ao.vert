uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;

uniform vec3 LightPosition;
uniform vec4 SkyColor;
uniform vec4 GroundColor;

attribute float AOAccessibility;
attribute vec4 Color;
varying vec4 VertexColor;

void main()
{
  vec4 eyePos = vec4(ModelViewMatrix * gl_Vertex);
  vec3 tnorm = normalize(NormalMatrix * gl_Normal);
  vec3 lightVec = normalize(LightPosition - eyePos.xyz);
  float cosTheta = dot(tnorm, lightVec);
  float a = cosTheta * 0.5 + 0.2;
  
  VertexColor = mix(Color, SkyColor, a) * AOAccessibility;
  gl_Position = ProjectionMatrix * eyePos;
}