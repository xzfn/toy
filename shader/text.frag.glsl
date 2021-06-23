#version 450

#include "frame_uniforms.glsl"

// FIXME 3 is material
layout(set = 3, binding = 1) uniform sampler2D u_Texture;

layout(location = 0) in vec3 v_Color;
layout(location = 1) in vec2 v_Texcoord;

layout(location = 0) out vec4 out_Color;

void main() {
	vec4 base_color_coeff = vec4(v_Color, 1.0f);
	vec4 base_color = texture(u_Texture, v_Texcoord);
	if (base_color.w == 0.0f) {
		discard;  // discard is not good, use it for now to avoid sorting
	}
	out_Color = base_color * base_color_coeff;
}
