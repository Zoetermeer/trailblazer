uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;

uniform vec3 LightPosition;
uniform vec3 HeadlightPosition;
uniform vec3 HeadlightDir;

uniform bool Animating;

//Clamped to [0.0, 1.0]
uniform float Time;

attribute float AOAccessibility;
attribute vec4 Color;
attribute vec3 VoxelCoordinate;
attribute vec3 TexCoords;

varying vec3 VaryingNormal;
varying vec3 VaryingLightDir;
varying vec3 VaryingHeadlightDir;
varying float VaryingAccessibility;
varying vec4 VertexColor;
varying vec3 VaryingVoxelCoordinate;
varying vec3 VaryingTexCoords;

const vec4 RED = vec4(1.0, 0.0, 0.0, 1.0);
const vec4 WHITE = vec4(1.0, 1.0, 1.0, 1.0);

float cubicEaseOut(float start, float end, float t)
{
  t--;
  return end * (t * t * t + 1.0) + start - 1.0;
}

vec3 cubicEaseOut(vec3 start, vec3 end, float t)
{
  return vec3(cubicEaseOut(start.x, end.x, t),
              cubicEaseOut(start.y, end.y, t),
              cubicEaseOut(start.z, end.z, t));
}

bool isEven(float f)
{
  return fract(f * .2) == 0.0;
}

void main()
{
  VaryingTexCoords = TexCoords;
  VaryingVoxelCoordinate = VoxelCoordinate;
  
  //Animate, if necessary
  vec3 vtx = gl_Vertex.xyz / gl_Vertex.w;
  if (Animating) {
    vec3 bot = vtx;
    float xdir = isEven(VoxelCoordinate.x) ? VoxelCoordinate.x : -VoxelCoordinate.x;
    float ydir = VoxelCoordinate.y;
    float zdir = isEven(VoxelCoordinate.z) ? VoxelCoordinate.z : -VoxelCoordinate.z;
    
    vec3 top = bot + vec3(xdir * 10.0, ydir * 10.0, zdir * 10.0);
    vec3 actTop = cubicEaseOut(bot, top, 1.0);
    
    float t = Time * 2.0;
    if (Time <= 0.5) {
      vtx = cubicEaseOut(bot, top, t);
      VertexColor = mix(Color, WHITE, t);
    } else {
      vtx = mix(actTop, bot, t - 1.0);
      VertexColor = mix(WHITE, Color, t - 1.0);
    }
  } else {
    VertexColor = Color;
  }
  
  vec4 eyePos = vec4(ModelViewMatrix * vec4(vtx, 1.0));
  
  //Defer AO to the frag shader to interpolate more smoothly
  VaryingNormal = NormalMatrix * gl_Normal;
  VaryingLightDir = LightPosition - eyePos.xyz;
  VaryingHeadlightDir = normalize(HeadlightPosition - eyePos.xyz);
  VaryingAccessibility = AOAccessibility;
  
  gl_Position = ProjectionMatrix * eyePos;
}