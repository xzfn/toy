#include "luawrap_light.h"

#include "light.h"


void luawrap_LightType(sol::table& m) {
	m.new_enum(
		"LightType",
		"Empty", LightType::Empty,
		"Point", LightType::Point,
		"Spot", LightType::Spot,
		"Directional", LightType::Directional
	);
}

void luawrap_Light(sol::table& m) {
	auto t = m.new_usertype<Light>("Light");
	t[sol::meta_function::construct] = sol::constructors<Light()>();

	t.set_function("get_type", &Light::get_type);
	t.set_function("set_type", &Light::set_type);

	t.set_function("get_transform", &Light::get_transform);
	t.set_function("set_transform", &Light::set_transform);

	t.set_function("get_color", &Light::get_color);
	t.set_function("set_color", &Light::set_color);

	t.set_function("get_range", &Light::get_range);
	t.set_function("set_range", &Light::set_range);

	t.set_function("get_spot_inner_angle", &Light::get_spot_inner_angle);
	t.set_function("set_spot_inner_angle", &Light::set_spot_inner_angle);

	t.set_function("get_spot_outer_angle", &Light::get_spot_outer_angle);
	t.set_function("set_spot_outer_angle", &Light::set_spot_outer_angle);
}

void luawrap_light(sol::table& m) {
	luawrap_LightType(m);
	luawrap_Light(m);
}
