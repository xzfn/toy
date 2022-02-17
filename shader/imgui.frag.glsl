#version 450

#include "frame_uniforms.glsl"

// FIXME 4 is material
layout(set = 4, binding = 1) uniform sampler2D u_Texture;

layout(location = 0) in vec4 v_Color;
layout(location = 1) in vec2 v_Texcoord;

layout(location = 0) out vec4 out_Color;

void main() {
	vec4 base_color_coeff = v_Color;
	vec4 base_color = texture(u_Texture, v_Texcoord);

	out_Color = base_color * base_color_coeff;
}
