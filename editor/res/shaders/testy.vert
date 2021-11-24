#version 460 core

in vec3 position;
in vec2 texCoord;

#uniform model mat4 uModel
uniform mat4 uModel;
#uniform view mat4 uView
uniform mat4 uView;
#uniform projection mat4 uProjection
uniform mat4 uProjection;

out Vertex {
  vec2 texCoord;
} OUT;

void main() {
  mat4 mvp = uProjection * uView * uModel;
  OUT.texCoord = texCoord;
  gl_Position = mvp * vec4(position, 1.0);
}
