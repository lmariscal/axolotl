vec3 rgb(vec3 color) {
  return color / 255.0f;
}

vec3 rgb(float r, float g, float b) {
  return vec3(r / 255.0f, g / 255.0f, b / 255.0f);
}

vec4 rgba(vec4 color) {
  return vec4(color.xyz / 255.0f, color.w);
}

vec4 rgba(float r, float g, float b, float a) {
  return vec4(r / 255.0f, g / 255.0f, b / 255.0f, a);
}

vec4 rgba(vec3 color, float a) {
  return vec4(color / 255.0f, a);
}
