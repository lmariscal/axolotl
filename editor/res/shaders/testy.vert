#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 texCoord;

#uniform model mat4 uModel
uniform mat4 uModel;
#uniform view mat4 uView
uniform mat4 uView;
#uniform projection mat4 uProjection
uniform mat4 uProjection;

out Vertex {
  vec3 normals;
  vec2 texCoord;
} OUT;

void main() {
  mat4 mvp = uProjection * uView * uModel;
  OUT.normals = normals;
  OUT.texCoord = texCoord;
  gl_Position = mvp * vec4(position, 1.0);
}
