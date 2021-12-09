#version 460 core

#include utils

layout(location = UNIFORM_TEXTURES) uniform sampler2D textures[TEXTURE_COUNT];

layout(std140) uniform Lights {
  int count;
  vec4 camera_position;
  Light data[LIGHT_COUNT];
}
lights;

layout(location = 0) in Vertex {
  vec3 position;
  vec2 tex_coord;
  mat3 tangent_matrix;
}
IN;

layout(location = 0) out vec4 frag_color;

void main() {
  vec4 ambient_light_color = lights.data[0].color * lights.data[0].intensity;

  // vec3 normal = normalize(IN.tangent_matrix * vec3(0.0, 0.0, 1.0)); // TODO: Replace with normal map
  vec3 normal = texture(textures[TEXTURE_NORMAL], IN.tex_coord).rgb;
  normal = normal * 2.0 - 1.0;
  normal = normalize(IN.tangent_matrix * normal);

  vec3 view_position = lights.camera_position.xyz;
  vec3 position = IN.position;

  vec4 diffuse_light_color = vec4(0.0);
  vec4 specular_light_color = vec4(0.0);

  Light directional_light = lights.data[1];
  vec3 light_direction = normalize(directional_light.position.xyz);
  float light_distance = length(position - light_direction);
  float light_intensity = max(0.0, dot(normal, light_direction));
  diffuse_light_color += directional_light.color * light_intensity;
  specular_light_color +=
    directional_light.color *
    pow(max(0.0, dot(normalize(view_position - position), reflect(-light_direction, normal))), 32.0);

  for (int i = 2; i < lights.count; i++) {
    Light light = lights.data[i];
    vec3 light_direction = normalize(light.position.xyz - position);
    float light_intensity = max(dot(normal, light_direction), 0.0);
    light_intensity = clamp(light_intensity, 0.0, 1.0) * light.intensity;
    diffuse_light_color += light.color * light_intensity;

    vec3 half_direction = normalize(light_direction + view_position);
    float specular_intensity = pow(clamp(dot(normal, half_direction), 0.0, 1.0), 32.0);
    specular_light_color += light.color * specular_intensity;
  }

  vec4 ambient_color = texture(textures[TEXTURE_AMBIENT], IN.tex_coord) * ambient_light_color;
  vec4 diffuse_color = texture(textures[TEXTURE_DIFFUSE], IN.tex_coord) * diffuse_light_color;
  vec4 specular_color = texture(textures[TEXTURE_SPECULAR], IN.tex_coord) * specular_light_color;
  specular_color = specular_color * 0.03f;

  frag_color = diffuse_color + ambient_color + specular_color;

  // frag_color = vec4(normal, 1.0);
}
