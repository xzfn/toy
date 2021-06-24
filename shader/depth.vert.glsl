#version 450

#include "frame_uniforms.glsl"
#include "push_constants.glsl"


layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_Texcoord;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() {
	gl_Position = u_frame.view_projection * u_push.model * vec4(in_Position, 1.0f);
}
