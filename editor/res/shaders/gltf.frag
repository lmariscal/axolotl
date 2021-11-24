#version 460 core

#include utils

#uniform other color color
uniform vec4 color;

out vec4 frag_color;

void main () {
  frag_color = color;
}
