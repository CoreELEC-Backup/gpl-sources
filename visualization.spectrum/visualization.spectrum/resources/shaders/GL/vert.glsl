#version 150

uniform mat4 u_projectionMatrix;
uniform mat4 u_modelViewMatrix;
uniform float u_pointSize;

in vec4 a_position;
in vec4 a_color;

out vec4 v_color;

void main ()
{
  gl_Position = u_projectionMatrix * u_modelViewMatrix * a_position;
  gl_PointSize = u_pointSize;
  v_color = a_color;
}
