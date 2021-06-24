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

vec3 encode_float(float n) {
	return vec3(n, n, n);
}

vec2 clip_to_uv(vec2 clip) {
	return (vec2(clip.x, -clip.y) + vec2(1.0, 1.0)) * 0.5;
}

vec3 shade_sun_light(vec3 base_color, vec3 view_dir, vec3 frag_normal, float visibility) {
	vec3 sun_light_color = vec3(1.0, 1.0, 1.0);
	vec3 light_dir = -u_frame.sun_light_direction;
	float ambient_strength = 0.2;
	float diffuse_strength = max(dot(frag_normal, light_dir), 0.0);
	vec3 reflect_dir = reflect(-light_dir, frag_normal);
	float specular_strength = pow(max(dot(reflect_dir, view_dir), 0.0f), 60)*0;
	return (ambient_strength + (diffuse_strength + specular_strength) * visibility) * sun_light_color * base_color;
}

void main() {
	vec3 base_color = texture(u_Texture, v_Texcoord).xyz;

	vec3 frag_normal = normalize(v_WorldNormal);

	vec3 view_dir = normalize(u_frame.camera_position - v_WorldPosition);

	vec4 clip_vec = u_shadow.layers[0].light_matrix * vec4(v_WorldPosition, 1.0);
	vec3 clip_pos = clip_vec.xyz / clip_vec.w;
	vec2 shadow_uv = clip_to_uv(clip_pos.xy);
	float shadow_depth = texture(u_ShadowMap, shadow_uv).x;
	float shadow_bias = 0.001;
	float shadow_delta = shadow_bias + shadow_depth - clip_pos.z;
	float visibility = step(0.0, shadow_delta);

	//out_Color = vec4(base_color * visibility, 1.0);
	vec3 shaded_color = shade_sun_light(base_color, view_dir, frag_normal, visibility);
	out_Color = vec4(shaded_color, 1.0);
	return;

	//float intensity = max(dot(u_frame.sun_light_direction, v_Normal), 0.0f);
	vec3 result_color = vec3(0.0f);

	for (int i = 0; i < u_light.light_count; ++i) {
		Light light = u_light.lights[i];
		if (light.type == 1) {
			vec3 light_dir = normalize(light.position - v_WorldPosition);
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
	out_Color = vec4(result_color * visibility, 1.0);
}
