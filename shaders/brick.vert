const float SPECULAR_CONTRIBUTION = 0.3;
const float DIFFUSE_CONTRIBUTION = 1.0 - SPECULAR_CONTRIBUTION;

varying float LightIntensity;
varying vec2 ModelPosition;

void main()
{ 
  float diffuse;
  vec3 lightVec;
  vec3 reflectVec;
  vec3 viewVec;
  vec3 lightPos = gl_LightSource[0].position.xyz;
  vec3 eyePos = vec3(gl_ModelViewMatrix * gl_Vertex);
  vec3 tnorm = normalize(gl_NormalMatrix * gl_Normal);
  lightVec = normalize(lightPos - eyePos);
  reflectVec = reflect(-lightVec, tnorm);
  viewVec = normalize(-eyePos);
  
  diffuse = max(dot(lightVec, tnorm), 0.0);
  float spec = 0.0;
  if (diffuse > 0.0) {
    spec = max(dot(reflectVec, viewVec), 0.0);
    spec = pow(spec, 16.0);
  }
  
  LightIntensity = DIFFUSE_CONTRIBUTION * diffuse +
                   SPECULAR_CONTRIBUTION * spec;
  ModelPosition = gl_Vertex.xy;
  
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}