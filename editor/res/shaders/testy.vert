#version 460 core

in vec3 position;

#uniform model mat4 uModel
uniform mat4 uModel;
#uniform view mat4 uView
uniform mat4 uView;
#uniform projection mat4 uProjection
uniform mat4 uProjection;

void main() {
  mat4 mvp = uProjection * uView * uModel;
  gl_Position = mvp * vec4(position, 1.0);
  // OUT.color  = color;
  // OUT.texCoord = texCoord;
  // gl_Position = vec4(position, 1.0f);
}
