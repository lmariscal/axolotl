#version 460 core

#include utils

layout(location = UNIFORM_TEXTURES) uniform sampler2D textures[TEXTURE_COUNT];

layout(std140) uniform Lights {
  int count;
  vec4 camera_position;
  Light data[LIGHT_COUNT];
} lights;

layout(location = 0) in Vertex {
  vec3 position;
  vec3 normal;
  vec2 tex_coord;
} IN;

layout(location = 0) out vec4 frag_color;

void main () {
  vec4 ambient_light_color = lights.data[0].color * lights.data[0].intensity;

  float specular_strength = 0.5;

  vec4 diffuse_light_color = vec4(0.0);
  vec4 specular_light_color = vec4(0.0);
  for (int i = 1; i < lights.count; i++) {
    Light light = lights.data[i];
    vec3 normal = normalize(IN.normal);
    vec3 light_direction = normalize(light.position.xyz - IN.position);
    float diffuse_intensity = max(dot(light_direction, normal), 0.0);
    diffuse_light_color += light.color * diffuse_intensity * light.intensity;

    vec3 view_direction = normalize(lights.camera_position.xyz - IN.position);
    vec3 reflection_direction = reflect(-light_direction, normal);
    float specular_intensity = pow(max(dot(view_direction, reflection_direction), 0.0), 32);
    specular_light_color += light.color * specular_intensity * light.intensity;

    specular_strength = max(specular_strength, specular_intensity);
  }

  vec4 light_color = ambient_light_color + diffuse_light_color + specular_light_color;
  frag_color = texture(textures[TEXTURE_DIFFUSE], IN.tex_coord) * light_color;
}
