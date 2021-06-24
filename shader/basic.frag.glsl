#version 450

#include "frame_uniforms.glsl"
#include "light_uniforms.glsl"
#include "shadow_uniforms.glsl"

layout(set = 2, binding = 1) uniform sampler2D u_ShadowMap;

// FIXME 4 is material
layout(set = 4, binding = 0) uniform MaterialUniforms {
	vec3 base_color;
}u_material;

layout(set = 4, binding = 1) uniform sampler2D u_Texture;
// layout(set = 4, binding = 2) uniform sampler2D u_Bump;

layout(location = 0) in vec3 v_WorldPosition;
layout(location = 1) in vec3 v_WorldNormal;
layout(location = 2) in vec2 v_Texcoord;

layout(location = 0) out vec4 out_Color;

vec3 encode_normal(vec3 normal) {
	return 0.5 + normal * 0.5f;
}

void main() {
	vec3 base_color = texture(u_ShadowMap, v_Texcoord).xxx;
	out_Color = vec4(base_color, 1.0);
	return;

	//float intensity = max(dot(u_frame.sun_light_direction, v_Normal), 0.0f);
	vec3 result_color = vec3(0.0f);

	vec3 frag_normal = normalize(v_WorldNormal);

	for (int i = 0; i < u_light.light_count; ++i) {
		Light light = u_light.lights[i];
		if (light.type == 1) {
			vec3 light_dir = normalize(light.position - v_WorldPosition);
			vec3 view_dir = normalize(u_frame.camera_position - v_WorldPosition);
			float diffuse_strength = max(dot(frag_normal, light_dir), 0.0);
			vec3 diffuse_color = diffuse_strength * base_color;
			result_color += diffuse_color * light.color;

			vec3 reflect_dir = reflect(-light_dir, frag_normal);
			float specular_strength = pow(max(dot(reflect_dir, view_dir), 0.0f), 60);
			vec3 specular_color = specular_strength * base_color;
			result_color += specular_color * light.color;
			//result_color = encode_normal(frag_normal);
		}
	}

	//base_color.xyz = u_frame.sun_light_direction;
	//base_color.xyz = normal_color;
	//base_color.xyz = u_material.base_color;
	out_Color = vec4(result_color, 1.0);
}
