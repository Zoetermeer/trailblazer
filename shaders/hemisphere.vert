uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;

uniform vec3 LightPosition;
uniform vec4 SkyColor;
uniform vec4 GroundColor;

varying vec4 VertexColor;

void main()
{
  vec3 eyePos = vec3(ModelViewMatrix * gl_Vertex);
  vec3 tnorm = normalize(NormalMatrix * gl_Normal);
  vec3 lightVec = normalize(LightPosition - eyePos);
  float cosTheta = dot(tnorm, lightVec);
  float a = cosTheta * 0.5 + 0.5;
  
  VertexColor = mix(GroundColor, SkyColor, a);
  gl_Position = ProjectionMatrix * (ModelViewMatrix * gl_Vertex);
}