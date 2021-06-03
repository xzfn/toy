#version 450

layout(set = 0, binding = 0) uniform FrameUniforms {
	mat4 u_view_projection;

	vec3 u_camera_position;
	float u_padding_0;

	vec4 u_sun_light_color;

	vec3 u_sun_light_direction;
	float u_padding_1;
};

layout(location = 0) in vec3 v_Color;

layout(location = 0) out vec4 out_Color;

void main() {
	vec4 base_color = vec4(v_Color, 1.0f);
	out_Color = base_color;
}
