uniform vec4 GroundColor;
uniform vec4 SkyColor;

varying vec3 VaryingNormal;
varying vec3 VaryingLightDir;
varying float VaryingAccessibility;
varying vec4 VertexColor;

void main()
{
  float cosTheta = dot(VaryingNormal, VaryingLightDir);
  float a = cosTheta * 0.5 + 0.2;
  
  gl_FragColor = mix(VertexColor, SkyColor, a) * VaryingAccessibility;
}