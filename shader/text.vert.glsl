#version 450

layout(set = 0, binding = 0) uniform FrameUniforms {
	mat4 u_view_projection;

	vec3 u_camera_position;
	float u_padding_0;

	vec4 u_sun_light_color;

	vec3 u_sun_light_direction;
	float u_padding_1;
};

layout(set = 1, binding = 0) uniform ModelUniforms {
	mat4 u_model;
};

layout(push_constant) uniform PushConstantBlock {
    mat4 model;
} u_push;

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Color;
layout(location = 2) in vec2 in_Texcoord;

out gl_PerVertex {
	vec4 gl_Position;
};

layout(location = 0) out vec3 v_Color;
layout(location = 1) out vec2 v_Texcoord;

void main() {
	gl_Position = u_view_projection * u_push.model * vec4(in_Position, 1.0f);
	v_Color = in_Color;
	v_Texcoord = in_Texcoord;
}
