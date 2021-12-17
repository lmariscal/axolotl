#version 460 core

#include utils

layout(location = 0) in Vertex {
  vec4 color;
}
IN;

layout(location = 0) out vec4 frag_color;

void main() {
  frag_color = IN.color;
}
