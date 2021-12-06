#version 460 core

#include utils

layout(location = ATTRIB_POSITION) in vec3 position;
layout(location = ATTRIB_NORMAL) in vec3 normal;
layout(location = ATTRIB_TANGENT) in vec3 tangent;
layout(location = ATTRIB_BITANGENT) in vec3 bitangent;
layout(location = ATTRIB_TEXCOORD) in vec2 tex_coord;

layout(location = UNIFORM_MODEL_MATRIX) uniform mat4 model;
layout(location = UNIFORM_VIEW_MATRIX) uniform mat4 view;
layout(location = UNIFORM_PROJECTION_MATRIX) uniform mat4 projection;

layout(location = 0) out Vertex {
  vec3 position;
  vec2 tex_coord;
  mat3 tangent_matrix;
} OUT;

void main() {
  mat4 mvp = projection * view * model;
  gl_Position = mvp * vec4(position, 1.0);

  OUT.position = vec3(model * vec4(position, 1.0));
  OUT.tex_coord = tex_coord;

  mat3 normal_matrix = transpose(inverse(mat3(model)));
  OUT.tangent_matrix = normal_matrix * mat3(tangent, bitangent, normal);

  // vec3 T = normalize(normal_matrix * tangent);
  // vec3 N = normalize(normal_matrix * normal);
  // T = normalize(T - dot(T, N) * N);
  // vec3 B = cross(N, T);

  // mat3 tangent_matrix = mat3(T, B, N);
  // OUT.tangent_matrix = tangent_matrix;
}
