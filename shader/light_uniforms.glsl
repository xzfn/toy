
#define MAX_LIGHTS 8


const int LightType_Empty = 0;
const int LightType_Point = 1;
const int LightType_Spot = 2;
const int LightType_Directional = 3;


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

	float range;
	float spot_inner;
	float spot_outer;
	float padding_6;
};


layout(set = 1, binding = 0) uniform LightUniforms {
	int light_count;
	float padding_0;
	float padding_1;
	float padding_2;

	Light lights[MAX_LIGHTS];
} u_light;
