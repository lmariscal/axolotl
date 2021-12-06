#version 460 core

#include utils

layout(location = ATTRIB_POSITION) in vec3 position;
layout(location = ATTRIB_NORMAL) in vec3 normal;
layout(location = ATTRIB_TANGENT) in vec3 tangent;
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

  mat3 transpose_inverse_model = transpose(inverse(mat3(model)));
  vec3 transformed_normal = transpose_inverse_model * normal;

  vec3 tangent = transpose_inverse_model * normalize(tangent);
  vec3 bitangent = normalize(cross(transformed_normal, tangent));
  OUT.tangent_matrix = mat3(tangent, bitangent, transformed_normal);
}
