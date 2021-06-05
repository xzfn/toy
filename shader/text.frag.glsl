#version 450

layout(set = 0, binding = 0) uniform FrameUniforms {
	mat4 u_view_projection;

	vec3 u_camera_position;
	float u_padding_0;

	vec4 u_sun_light_color;

	vec3 u_sun_light_direction;
	float u_padding_1;
};

layout(set = 2, binding = 1) uniform sampler2D u_Texture;

layout(location = 0) in vec3 v_Color;
layout(location = 1) in vec2 v_Texcoord;

layout(location = 0) out vec4 out_Color;

void main() {
	vec4 base_color_coeff = vec4(v_Color, 1.0f);
	vec4 base_color = texture(u_Texture, v_Texcoord);
	out_Color = base_color * base_color_coeff;
}
