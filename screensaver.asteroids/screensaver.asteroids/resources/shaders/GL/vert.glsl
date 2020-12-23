#version 150

uniform mat4 u_modelViewProjectionMatrix;

in vec3 a_position;
in vec4 a_color;

out vec4 v_color;

void main ()
{
  gl_Position = u_modelViewProjectionMatrix * vec4(a_position, 1.0);
  v_color = a_color;
}
