
#define MAX_LIGHTS 8

struct Light {
	int type;
	float padding_0;
	float padding_1;
	float padding_2;

	vec3 color;
	float padding_3;

	vec3 position;
	float padding_4;

	vec3 direction;
	float padding_5;
};


layout(set = 1, binding = 0) uniform LightUniforms {
	int light_count;
	float padding_0;
	float padding_1;
	float padding_2;

	Light lights[MAX_LIGHTS];
} u_light;
