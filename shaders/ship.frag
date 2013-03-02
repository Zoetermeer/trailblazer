uniform vec4 AmbientColor;
uniform vec4 DiffuseColor;
uniform vec4 SpecularColor;

varying vec3 Normal;
varying vec3 Vertex;
varying vec2 ModelPosition;

const vec4 cyan = vec4(0.0, 1.0, 1.0, 1.0);

bool inRange(float f)
{
  float fr = floor(f * 100.0);
  if (fract(fr / 10.0) == 0.0)
    return true;
  
  return false;
}

void main()
{
  float fractY = fract(ModelPosition.y);
  float fractX = fract(ModelPosition.x);
  if (inRange(fractY) && inRange(fractX)) {
    gl_FragColor = cyan;
  } else {
    //Do the normal Phong calculation
    vec3 L = normalize(gl_LightSource[0].position.xyz - Vertex);
    vec3 E = normalize(-Vertex); // we are in Eye Coordinates, so EyePos is (0,0,0)
    vec3 R = normalize(-reflect(L,Normal));
    
    //Ambient term
    vec4 amb = gl_FrontLightProduct[0].ambient;
    
    //Diffuse term
    vec4 diff = gl_FrontLightProduct[0].diffuse * max(dot(Normal,L), 0.0);
    diff = clamp(diff, 0.0, 1.0);
    
    //Specular term
    vec4 spec = gl_FrontLightProduct[0].specular
    * pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);
    spec = clamp(spec, 0.0, 1.0);
    
    gl_FragColor = gl_FrontLightModelProduct.sceneColor + amb + diff + spec;
  }
}






