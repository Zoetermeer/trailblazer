uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;
uniform vec3 LightPosition;

const vec4 AmbientColor = vec4(0.4, 0.4, 0.4, 1.0);
const vec4 DiffuseColor = vec4(0.4, 0.4, 0.4, 1.0);
const vec4 SpecularColor = vec4(0.4, 0.4, 0.4, 1.0);

varying vec4 VertexColor;

void main()
{
  vec3 eyeNorm = NormalMatrix * gl_Normal;
  vec4 worldVert = ModelViewMatrix * gl_Vertex;
  vec3 pos3 = gl_Vertex.xyz / worldVert.w;
  
  vec3 toLight = normalize(LightPosition - pos3);
  float diff = max(0.0, dot(eyeNorm, toLight));
  VertexColor.xyz = diff * DiffuseColor.xyz;
  VertexColor.a = 1.0;
  
  //Ambient light
  VertexColor += AmbientColor;
  
  //Specular
  vec3 reflectVec = normalize(reflect(-toLight, eyeNorm));
  float spec = max(0.0, dot(eyeNorm, reflectVec));
  if (diff != 0.0) {
    float fspec = pow(spec, 128.0);
    VertexColor.rgb += vec3(fspec, fspec, fspec);
  }
  
  gl_Position = ProjectionMatrix * (ModelViewMatrix * gl_Vertex);
}