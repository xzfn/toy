#version 450

layout(set = 0, binding = 1) uniform u_UniformBuffer {
	mat4 u_Projection;
};

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_Texcoord;

out gl_PerVertex {
	vec4 gl_Position;
};

layout(location = 0) out vec3 v_Normal;
layout(location = 1) out vec2 v_Texcoord;

void main() {
	gl_Position = u_Projection * vec4(in_Position, 1.0f);
	gl_Position.y = gl_Position.y;
	v_Texcoord = in_Texcoord;
	v_Normal = in_Normal;
}
