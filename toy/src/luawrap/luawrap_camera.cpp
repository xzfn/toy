#include "luawrap_camera.h"

#include "camera.h"


void luawrap_PerspectiveData(sol::table& m) {
	auto t = m.new_usertype<PerspectiveData>("PerspectiveData");
	t[sol::meta_function::construct] = sol::constructors<PerspectiveData()>();
	t.set("fov", &PerspectiveData::fov);
	t.set("aspect", &PerspectiveData::aspect);
	t.set("z_near", &PerspectiveData::z_near);
	t.set("z_far", &PerspectiveData::z_far);
}

void luawrap_OrthographicData(sol::table& m) {
	auto t = m.new_usertype<OrthographicData>("OrthographicData");
	t[sol::meta_function::construct] = sol::constructors<OrthographicData()>();
	t.set("left", &OrthographicData::left);
	t.set("right", &OrthographicData::right);
	t.set("bottom", &OrthographicData::bottom);
	t.set("top", &OrthographicData::top);
	t.set("z_near", &OrthographicData::z_near);
	t.set("z_far", &OrthographicData::z_far);
}

void luawrap_Camera(sol::table& m) {
	auto t = m.new_usertype<Camera>("Camera");
	t[sol::meta_function::construct] = sol::no_constructor;
	t.set_function("set_view", &Camera::set_view);
	t.set_function("set_perspective_param", &Camera::set_perspective_param);
	t.set_function("set_orthographic_param", &Camera::set_orthographic_param);
	t.set_function("set_perspective_data", &Camera::set_perspective_data);
	t.set_function("set_orthographic_data", &Camera::set_orthographic_data);
	t.set_function("get_perspective_data", &Camera::get_perspective_data);
	t.set_function("get_orthographic_data", &Camera::get_orthographic_data);
	t.set_function("is_perspective", &Camera::is_perspective);
	t.set_function("set_perspective", &Camera::set_perspective);
	t.set_function("get_view", &Camera::get_view);
	t.set_function("get_projection", &Camera::get_projection);
	t.set_function("get_view_projection", &Camera::get_view_projection);
	t.set_function("screen_to_world", &Camera::screen_to_world);
	t.set_function("screen_to_world_ray", &Camera::screen_to_world_ray);
	t.set_function("world_to_screen", &Camera::world_to_screen);
}

void luawrap_camera(sol::table& m) {
	luawrap_PerspectiveData(m);
	luawrap_OrthographicData(m);
	luawrap_Camera(m);
}
