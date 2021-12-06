#version 460 core

#include utils

layout(location = UNIFORM_TEXTURES) uniform sampler2D textures[TEXTURE_COUNT];

// struct Light {
//   vec4 position;
//   vec4 color;
//   float intensity;
// };

layout(std140) uniform Lights {
  int count;
  vec4 camera_position;
  Light data[LIGHT_COUNT];
} lights;

layout(location = 0) in Vertex {
  vec3 position;
  vec2 tex_coord;
  mat3 tangent_matrix;
} IN;

layout(location = 0) out vec4 frag_color;

void main () {
  vec4 color = texture(textures[TEXTURE_DIFFUSE], IN.tex_coord);
  vec3 normal = texture(textures[TEXTURE_NORMAL], IN.tex_coord).rgb;
  vec3 tangent = normalize(IN.tangent_matrix * normal);
  vec3 bitangent = cross(normal, tangent);
  vec3 light_dir = normalize(lights.data[1].position.xyz - IN.position);
  float diffuse = max(dot(light_dir, tangent), 0.0);
  float specular = pow(max(dot(light_dir, bitangent), 0.0), 16.0);
  frag_color = vec4(color.rgb * (diffuse + specular) * lights.data[1].color.rgb, color.a);
}

  // vec4 ambient_light_color = lights.data[0].color * lights.data[0].intensity;

  // vec3 normal = texture(textures[TEXTURE_NORMAL], IN.tex_coord).rgb;
  // // normal = normal * 2.0 - 1.0;
  // normal = normalize(IN.tangent_matrix * normal);

  // vec3 view_position = lights.camera_position.xyz;
  // vec3 position = IN.position;

  // vec4 diffuse_light_color = vec4(0.0);
  // vec4 specular_light_color = vec4(0.0);
  // for (int i = 1; i < lights.count; i++) {
  //   Light light = lights.data[i];
  //   vec3 light_direction = normalize(light.position.xyz - position);
  //   float light_distance = length(light.position.xyz - position);
  //   float light_intensity = light.intensity / (light_distance * light_distance);
  //   float light_angle = max(dot(normal, light_direction), 0.0);

  //   diffuse_light_color += light.color * light_intensity * light_angle;
  // }

    // vec3 light_position = light.position.xyz;
    // vec3 light_direction = normalize(light_position - position);
    // float diffuse_intensity = max(dot(light_direction, normal), 0.0);
    // diffuse_light_color += light.color * diffuse_intensity * light.intensity;

    // vec3 view_direction = normalize(view_position - position);
    // vec3 halfway_direction = normalize(light_direction + view_direction);
    // float specular_intensity = pow(max(dot(normal, halfway_direction), 0.0), 32);
    // specular_light_color += light.color * specular_intensity * light.intensity;
  // }

  // vec4 ambient_color = texture(textures[TEXTURE_AMBIENT], IN.tex_coord) * ambient_light_color;
  // vec4 diffuse_color = texture(textures[TEXTURE_DIFFUSE], IN.tex_coord) * diffuse_light_color;
  // vec4 specular_color = texture(textures[TEXTURE_SPECULAR], IN.tex_coord) * specular_light_color;
  // vec4 light_color = ambient_color + diffuse_color + specular_color;
  // frag_color = light_color;
// }
