uniform vec4 AmbientColor;
uniform vec4 DiffuseColor;
uniform vec4 SpecularColor;
uniform bool HeadlightOn;

varying vec3 VaryingNormal;
varying vec3 VaryingLightDir;
varying vec3 VaryingHeadlightDir;

vec4 computePhong(vec3 lightDir, bool spotLight)
{
  float diff = max(0.0, dot(normalize(VaryingNormal), normalize(lightDir)));
  if (spotLight && diff > 0.2) {
    diff = max(0.0, diff - 0.5);
  }
  
  vec4 color = diff * DiffuseColor;
  color = color + AmbientColor;
  
  vec3 reflectVec = normalize(reflect(-normalize(lightDir), normalize(VaryingNormal)));
  float spec = max(0.0, dot(normalize(VaryingNormal), reflectVec));
  
  if (diff != 0.0) {
    float fspec = pow(spec, 32.0);
    color.rgb += vec3(fspec, fspec, fspec);
  }
  
  return color;
}

void main()
{
  vec4 mainLightColor = computePhong(VaryingLightDir, false);
  if (HeadlightOn) {
    vec4 headLightColor = computePhong(VaryingHeadlightDir, true);
    gl_FragColor = mix(mainLightColor, headLightColor, 0.5);
    return;
  }
  
  gl_FragColor = mainLightColor;
}












