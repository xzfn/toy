#version 450

#include "frame_uniforms.glsl"
#include "push_constants.glsl"


layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_Texcoord;

out gl_PerVertex {
	vec4 gl_Position;
};

layout(location = 0) out vec3 v_WorldPosition;
layout(location = 1) out vec3 v_WorldNormal;
layout(location = 2) out vec2 v_Texcoord;

void main() {
	v_WorldPosition = (u_push.model * vec4(in_Position, 1.0f)).xyz;
	v_WorldNormal = (u_push.model * vec4(in_Normal, 0.0f)).xyz;
	gl_Position = u_frame.view_projection * u_push.model * vec4(in_Position, 1.0f);
	v_Texcoord = in_Texcoord;
}
