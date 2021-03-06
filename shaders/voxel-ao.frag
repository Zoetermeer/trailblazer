uniform vec4 GroundColor;
uniform vec4 SkyColor;
uniform bool HeadlightOn;

uniform sampler2D Texture;

varying vec3 VaryingNormal;
varying vec3 VaryingLightDir;
varying vec3 VaryingHeadlightDir;
varying float VaryingAccessibility;
varying vec4 VertexColor;
varying vec3 VaryingTexCoords;
varying vec3 VaryingVoxelCoordinate;

void main()
{
  float cosTheta = dot(normalize(VaryingNormal), normalize(VaryingLightDir));
  float a = cosTheta * 0.5 + 0.2;
  if (HeadlightOn) {
    float diff = max(0.0, dot(-VaryingHeadlightDir, VaryingNormal));
    a = max(0.0, a - diff);
  }
  
  vec4 color = mix(VertexColor, SkyColor, a) * VaryingAccessibility;
  vec4 texColor = mix(texture2D(Texture, VaryingTexCoords.st), texture2D(Texture, VaryingVoxelCoordinate.xz / 5096.0), 0.2);
  
  gl_FragColor = mix(color, texColor, 0.5);
}