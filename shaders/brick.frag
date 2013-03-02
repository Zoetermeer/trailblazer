const vec3 BrickColor = vec3(0.8, 0.2, 0.0);
const vec3 MortarColor = vec3(1.0, 1.0, 1.0);
const vec2 BrickSize = vec2(0.8, 0.5);
const vec2 BrickPct = vec2(0.5, 0.8);

varying vec2 ModelPosition;
varying float LightIntensity;

void main()
{
  vec3 color;
  vec2 position, useBrick;
  
  position = ModelPosition / BrickSize;
  if (fract(position.y * 0.5) > 0.5) 
    position.x += 0.5;
    
  position = fract(position);
  useBrick = step(position, BrickPct);
  
  color = mix(MortarColor, BrickColor, useBrick.x * useBrick.y);
  color *= LightIntensity;
  gl_FragColor = vec4(color, 1.0);
}