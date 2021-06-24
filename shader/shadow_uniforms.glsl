
#define MAX_SHADOWS 8

struct ShadowLayer {
	mat4 light_matrix;
};

layout(set = 4, binding = 0) uniform ShadowUniforms {
	ShadowLayer shadows[MAX_SHADOWS];
} u_shadow;
