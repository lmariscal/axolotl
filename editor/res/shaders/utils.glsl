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

struct Light {
  vec4 position;
  vec4 color;
  float intensity;
};

// Texture indices
#define TEXTURE_DIFFUSE   0
#define TEXTURE_SPECULAR  1
#define TEXTURE_NORMAL    2
#define TEXTURE_AMBIENT   3
#define TEXTURE_HEIGHT    4
#define TEXTURE_BUFFER    5
#define TEXTURE_COUNT     6

// Vertex uniform locations
#define UNIFORM_MODEL_MATRIX      0
#define UNIFORM_VIEW_MATRIX       1
#define UNIFORM_PROJECTION_MATRIX 2
#define UNIFORM_TIME              3
#define UNIFORM_RESOLUTION        4
#define UNIFORM_MOUSE             5
#define UNIFORM_CUSTOM_VERTEX     6

// Fragment uniform locations
#define UNIFORM_TEXTURES        10 // 10 - 16
#define UNIFORM_SKYBOX          17
#define UNIFORM_LIGHTS          18 // 18 - 49
#define UNIFORM_CUSTOM_FRAGMENT 50

// Vertex attribute locations
#define ATTRIB_POSITION 0
#define ATTRIB_NORMAL   1
#define ATTRIB_TEXCOORD 2

#define LIGHT_COUNT 32
