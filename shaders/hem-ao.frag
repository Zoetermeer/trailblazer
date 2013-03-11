uniform vec4 GroundColor;
uniform vec4 SkyColor;
uniform bool HeadlightOn;

varying vec3 VaryingNormal;
varying vec3 VaryingLightDir;
varying vec3 VaryingHeadlightDir;
varying float VaryingAccessibility;
varying vec4 VertexColor;

void main()
{
  float cosTheta = dot(normalize(VaryingNormal), normalize(VaryingLightDir));
  float a = cosTheta * 0.5 + 0.2;
  if (HeadlightOn) {
    float diff = max(0.0, dot(-VaryingHeadlightDir, VaryingNormal));
    a = max(0.0, a - diff);
  }
  
  gl_FragColor = mix(VertexColor, SkyColor, a) * VaryingAccessibility;
}