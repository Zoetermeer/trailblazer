const vec4 AmbientColor = vec4(0.4, 0.4, 0.4, 1.0);
const vec4 DiffuseColor = vec4(0.4, 0.4, 0.4, 1.0);
const vec4 SpecularColor = vec4(0.4, 0.4, 0.4, 1.0);

varying vec4 Color;

void main()
{
  vec3 eyeNorm = gl_NormalMatrix * gl_Normal;
  vec4 worldVert = gl_ModelViewMatrix * gl_Vertex;
  vec3 pos3 = gl_Position.xyz / worldVert.w;
  
  vec3 toLight = normalize(gl_LightSource[0].position - worldVert).xyz;
  float diff = max(0.0, dot(eyeNorm, toLight));
  Color.xyz = diff * DiffuseColor.xyz;
  Color.a = 1.0;
  
  //Ambient light
  Color += AmbientColor;
  
  //Specular
  vec3 reflectVec = normalize(reflect(-toLight, eyeNorm));
  float spec = max(0.0, dot(eyeNorm, reflectVec));
  if (diff != 0.0) {
    float fspec = pow(spec, 128.0);
    Color.rgb += vec3(fspec, fspec, fspec);
  }
  
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}