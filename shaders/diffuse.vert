varying vec4 Color;

const vec4 DiffuseColor = vec4(1.0, 1.0, 1.0, 1.0);

void main()
{
  vec3 eyeNorm = gl_NormalMatrix * gl_Normal;
  vec4 pos4 = gl_ModelViewMatrix * gl_Vertex;
  vec3 pos3 = gl_Position.xyz / pos4.w;
  
  vec3 toLight = normalize(gl_LightSource[0].position - pos4).xyz;
  float diff = max(0.0, dot(eyeNorm, toLight));
  Color.xyz = diff * DiffuseColor.xyz;
  Color.a = 1.0;
  
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}