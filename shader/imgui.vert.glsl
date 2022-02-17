#version 450

#include "frame_uniforms.glsl"
#include "push_constants.glsl"


layout(location = 0) in vec2 in_Position;
layout(location = 1) in vec2 in_Texcoord;
layout(location = 2) in vec4 in_Color;

out gl_PerVertex {
	vec4 gl_Position;
};

layout(location = 0) out vec4 v_Color;
layout(location = 1) out vec2 v_Texcoord;

vec2 screen_to_clip(vec2 screen_position) {
	float clip_x = screen_position.x / u_frame.screen_width * 2.0f - 1.0f;
	float clip_y = (1.0 - screen_position.y / u_frame.screen_height) * 2.0f - 1.0f;
	return vec2(clip_x, clip_y);
}

void main() {
	gl_Position = vec4(screen_to_clip(in_Position), 0.0f, 1.0f);
	// gl_Position = u_push.model * vec4(in_Position, 0.0f, 1.0f);

	v_Color = in_Color;
	v_Texcoord = in_Texcoord;
}
