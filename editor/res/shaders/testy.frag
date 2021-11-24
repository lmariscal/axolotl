#version 460 core

#include utils

#uniform other color color
uniform vec4 color;
#uniform texture0 sampler2D diffuse1
uniform sampler2D diffuse1;
#uniform texture1 sampler2D diffuse2
uniform sampler2D diffuse2;
#uniform texture2 sampler2D diffuse3
uniform sampler2D diffuse3;
#uniform texture3 sampler2D specular1
uniform sampler2D specular1;
#uniform texture4 sampler2D specular2
uniform sampler2D specular2;

in Vertex {
  vec3 normals;
  vec2 texCoord;
} IN;

out vec4 fragColor;

void main () {
  fragColor = texture(diffuse1, IN.texCoord) * color;
}
