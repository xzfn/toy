
layout(set = 0, binding = 0) uniform FrameUniforms {
	mat4 view_projection;

	vec3 camera_position;
	float padding_0;

	vec4 sun_light_color;

	vec3 sun_light_direction;
	float padding_1;
} u_frame;
