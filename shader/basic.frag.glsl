#version 450

#include "frame_uniforms.glsl"
#include "light_uniforms.glsl"
#include "shadow_uniforms.glsl"

layout(set = 2, binding = 1) uniform sampler2DArray u_ShadowMap;

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

vec3 encode_small_int(int n) {
	const vec3 rainbow[7] = {
		vec3(1.0, 0.0, 0.0),  // FF0000  red
		vec3(1.0, 0.4980, 0.0),  // FF7F00  orange
		vec3(1.0, 1.0, 0.0),  // FFFF00  yellow
		vec3(0.0, 1.0, 0.0),  // 00FF00  green
		vec3(0.0, 1.0, 1.0),  // 00FFFF  cyan
		vec3(0.0, 0.0, 1.0),  // 0000FF  blue
		vec3(0.5451, 0.0, 1.0)  // 8B00FF  violet
	};
	return rainbow[n % 7];
}

vec2 clip_to_uv(vec2 clip) {
	return (vec2(clip.x, -clip.y) + vec2(1.0, 1.0)) * 0.5;
}

int calc_sun_cascade(vec4 sun_cascade_splits, float view_z) {
	bvec4 bools = greaterThan(vec4(-view_z), sun_cascade_splits);
	return int(bools.x) + int(bools.y) + int(bools.z) + int(bools.w);
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

float calc_shadow_visibility(uint layer) {
	vec4 clip_vec = u_shadow.layers[layer].light_matrix * vec4(v_WorldPosition, 1.0);
	vec3 clip_pos = clip_vec.xyz / clip_vec.w;
	vec2 shadow_uv = clip_to_uv(clip_pos.xy);
	float shadow_depth = texture(u_ShadowMap, vec3(shadow_uv, layer)).x;
	float shadow_bias = 0.001;
	float shadow_delta = shadow_bias + shadow_depth - clip_pos.z;
	float visibility = step(0.0, shadow_delta);
	return visibility;
}

float calc_spot_angle_attenuation(float cos_theta, float cos_inner, float cos_outer) {
	return clamp((cos_theta - cos_outer) / (cos_inner - cos_outer), 0.0, 1.0);
}

vec3 apply_fog(vec3 shaded_color, vec3 fog_color, float fog_density, float distance_) {
	float f = exp(-fog_density * distance_);
	return mix(fog_color, shaded_color, f);
}

void main() {
	vec3 base_color = texture(u_Texture, v_Texcoord).xyz;

	vec3 frag_normal = normalize(v_WorldNormal);

	vec3 view_vec = u_frame.camera_position - v_WorldPosition;
	vec3 view_dir = normalize(u_frame.camera_position - v_WorldPosition);

	float view_z = (u_frame.view * vec4(v_WorldPosition, 1.0)).z;

	int sun_shadow_layer = calc_sun_cascade(u_frame.sun_cascade_splits, view_z);
	vec3 debug_cascade_color = encode_small_int(sun_shadow_layer);
	float visibility = calc_shadow_visibility(sun_shadow_layer);

	//out_Color = vec4(base_color * visibility, 1.0);
	vec3 shaded_color = 0.5 * shade_sun_light(base_color, view_dir, frag_normal, visibility);
	//out_Color = vec4(shaded_color, 1.0);
	//return;

	//float intensity = max(dot(u_frame.sun_light_direction, v_Normal), 0.0f);
	vec3 result_color = vec3(0.0f);

	for (int i = 0; i < u_light.light_count; ++i) {
		Light light = u_light.lights[i];
		if (light.type == LightType_Point) {
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
		else if (light.type == LightType_Spot) {
			vec3 spot_dir = light.direction;

			float spot_visibility = 1.0f;
			if (bool(light.shadow)) {
				spot_visibility = calc_shadow_visibility(light.shadow_layer);
			}

			vec3 light_dir = normalize(light.position - v_WorldPosition);

			float cos_theta = dot(spot_dir, -light_dir);
			float spot_attenuation = calc_spot_angle_attenuation(cos_theta, light.spot_inner, light.spot_outer);

			float diffuse_strength = max(dot(frag_normal, light_dir), 0.0);
			vec3 diffuse_color = diffuse_strength * base_color;
			result_color += diffuse_color * light.color * spot_attenuation * spot_visibility;

			vec3 reflect_dir = reflect(-light_dir, frag_normal);
			float specular_strength = pow(max(dot(reflect_dir, view_dir), 0.0f), 60);
			vec3 specular_color = specular_strength * base_color;
			result_color += specular_color * light.color * spot_attenuation * spot_visibility;
		}
	}
	
	shaded_color = shaded_color + 0.5 * result_color;

	vec3 fog_color = vec3(0.4);
	shaded_color = apply_fog(shaded_color, fog_color, 0.05, length(view_vec));

	out_Color = vec4(shaded_color, 1.0);
	out_Color = out_Color;// * vec4(debug_cascade_color, 1.0);
}
