#version 450

#include "frame_uniforms.glsl"
#include "push_constants.glsl"


layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_Texcoord;

layout(location = 0) out vec3 v_Texcoord;


void main() {
	vec4 pos = u_frame.view_projection * u_push.model * vec4(in_Position, 1.0f);
	gl_Position = pos.xyww;
	v_Texcoord = in_Position;
}
