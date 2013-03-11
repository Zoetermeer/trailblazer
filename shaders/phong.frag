uniform vec4 AmbientColor;
uniform vec4 DiffuseColor;
uniform vec4 SpecularColor;

varying vec3 VaryingNormal;
varying vec3 VaryingLightDir;

void main()
{
  float diff = max(0.0, dot(normalize(VaryingNormal), normalize(VaryingLightDir)));
  vec4 color = diff * DiffuseColor;
  color = color + AmbientColor;
  
  vec3 reflectVec = normalize(reflect(-normalize(VaryingLightDir), normalize(VaryingNormal)));
  float spec = max(0.0, dot(normalize(VaryingNormal), reflectVec));
  
  if (diff != 0.0) {
    float fspec = pow(spec, 128.0);
    color.rgb += vec3(fspec, fspec, fspec);
  }
  
  gl_FragColor = color;
}












