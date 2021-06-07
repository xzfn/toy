#version 450

#include "frame_uniforms.glsl"
#include "push_constants.glsl"


layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Color;
layout(location = 2) in vec2 in_Texcoord;

out gl_PerVertex {
	vec4 gl_Position;
};

layout(location = 0) out vec3 v_Color;
layout(location = 1) out vec2 v_Texcoord;

vec3 screen_to_clip(vec3 screen_position) {
	float clip_x = screen_position.x / u_frame.screen_width * 2.0f - 1.0f;
	float clip_y = screen_position.y / u_frame.screen_height * 2.0f - 1.0f;
	return vec3(clip_x, clip_y, screen_position.z);
}

void main() {
	gl_Position = vec4(screen_to_clip(in_Position), 1.0f);

	v_Color = in_Color;
	v_Texcoord = in_Texcoord;
}
