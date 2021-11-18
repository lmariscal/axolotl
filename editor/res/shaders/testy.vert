#version 150 core

#include utils

in vec3 position;
in vec4 colour;
in vec2 texCoord;

out Vertex {
  vec2 texCoord;
  vec4 colour;
} OUT;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {
  mat4 mvp = uProjection * uView * uModel;
  gl_Position = mvp * vec4(position, 1.0);
  OUT.colour  = colour;
  OUT.texCoord = texCoord;
}
