
#define MAX_SHADOWS 8

struct ShadowLayer {
	mat4 light_matrix;
};

// FIXME shadow set index
layout(set = 2, binding = 0) uniform ShadowUniforms {
	ShadowLayer layers[MAX_SHADOWS];
} u_shadow;
