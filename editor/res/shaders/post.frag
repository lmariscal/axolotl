#version 460 core

#include utils

layout(location = 40) uniform sampler2D tex;
layout(location = 42) uniform vec2 viewport_size;

layout(location = 0) in Vertex {
  vec2 tex_coord;
}
IN;

layout(location = 0) out vec4 frag_color;

void main() {
  frag_color = texture(tex, IN.tex_coord);

  frag_color.rgb = pow(frag_color.rgb, vec3(1.8));

  // vignette
  float dist = length(gl_FragCoord.xy / viewport_size - vec2(0.5));
  frag_color.rgb *= smoothstep(0.8, 0.5, dist);
}
