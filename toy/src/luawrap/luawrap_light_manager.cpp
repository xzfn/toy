#include "luawrap_light_manager.h"

#include "light_manager.h"


void luawrap_light_manager(sol::table& m) {
	auto t = m.new_usertype<LightManager>("LightManager");
	t[sol::meta_function::construct] = sol::no_constructor;

	t.set_function("add_light", &LightManager::add_light);
	t.set_function("remove_light", &LightManager::remove_light);
	t.set_function("set_sun", &LightManager::set_sun);
	t.set_function("get_sun", &LightManager::get_sun);
}
