uniform vec4 AmbientColor;
uniform vec4 DiffuseColor;
uniform vec4 SpecularColor;

varying vec3 Normal;
varying vec3 Vertex;

void main (void)
{
  //Light at origin in eye coordinates?
  //vec3 lightPos = vec3(0.0, 0.0, 0.0);
  vec3 L = normalize(gl_LightSource[0].position.xyz - Vertex);
  
  //vec3 L = normalize(lightPos - Vertex);
  vec3 E = normalize(-Vertex); // we are in eye coordinates, so EyePos is (0,0,0)
  vec3 R = normalize(-reflect(L, Normal));
  
  //Ambient term
  //vec4 amb = gl_FrontLightProduct[0].ambient;
  vec4 amb = AmbientColor;
  amb = clamp(amb, 0.0, 1.0);
  
  //Diffuse term
  //vec4 diff = DiffuseColor * max(dot(Normal, L), 0.0);
  vec4 diff = gl_FrontLightProduct[0].diffuse * max(dot(Normal, L), 0.0);
  //vec4 diff = clamp(DiffuseColor, 0.0, 1.0);
  
  
  //Specular term
  vec4 spec = SpecularColor * pow(max(dot(R, E), 0.0), 8.0);
  spec = clamp(spec, 0.0, 1.0);

  vec4 color = gl_FrontLightModelProduct.sceneColor + amb + diff + spec;
  gl_FragColor = clamp(color, 0.3, 1.0);
}