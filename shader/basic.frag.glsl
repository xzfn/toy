#version 450

layout(set = 0, binding = 0) uniform FrameUniforms {
	mat4 u_view_projection;

	vec3 u_camera_position;
	float u_padding_0;

	vec4 u_sun_light_color;

	vec3 u_sun_light_direction;
	float u_padding_1;
};

layout(set = 2, binding = 0) uniform MaterialUniforms {
	vec3 u_base_color;
};

layout(set = 2, binding = 1) uniform sampler2D u_Texture;
// layout(set = 2, binding = 2) uniform sampler2D u_Bump;

layout(location = 0) in vec3 v_Normal;
layout(location = 1) in vec2 v_Texcoord;

layout(location = 0) out vec4 out_Color;

vec3 encode_normal(vec3 normal) {
	return 0.5 + normal * 0.5f;
}

void main() {
	vec4 base_color = texture(u_Texture, v_Texcoord);
	vec3 normal_color = encode_normal(v_Normal);
	float intensity = dot(u_sun_light_direction, v_Normal);
	base_color.xyz = base_color.xyz * intensity * u_base_color;
	
	//base_color.xyz = normal_color;
	out_Color = base_color;
}
