#version 460 core

in vec3 position;

#uniform model mat4 model
uniform mat4 model;
#uniform view mat4 view
uniform mat4 view;
#uniform projection mat4 projection
uniform mat4 projection;

void main() {
  mat4 mvp = projection * view * model;
  gl_Position = mvp * vec4(position, 1.0);
}
