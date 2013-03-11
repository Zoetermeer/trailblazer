uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform vec4 Color;

varying vec4 VertexColor;

void main()
{
  gl_Position = ProjectionMatrix * (ModelViewMatrix * gl_Vertex);
  VertexColor = Color;
}