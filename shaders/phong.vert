uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;

varying vec3 Normal;
varying vec3 Vertex;

void main(void)
{
  Vertex = vec3(ModelViewMatrix * gl_Vertex);
  Normal = normalize(NormalMatrix * gl_Normal);
  gl_Position = ProjectionMatrix * (ModelViewMatrix * gl_Vertex);
}