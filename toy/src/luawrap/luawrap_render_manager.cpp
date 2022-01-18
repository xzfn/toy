#include "luawrap_render_manager.h"

#include "render_manager.h"


void luawrap_PartSlashTotal(sol::table& m) {
	auto t = m.new_usertype<PartSlashTotal>("PartSlashTotal");
	t[sol::meta_function::construct] = sol::no_constructor;
	t.set("part", sol::readonly(&PartSlashTotal::part));
	t.set("total", sol::readonly(&PartSlashTotal::total));
}

void luawrap_RenderManagerDebugInfo(sol::table& m) {
	auto t = m.new_usertype<RenderManagerDebugInfo>("RenderManagerDebugInfo");
	t[sol::meta_function::construct] = sol::no_constructor;
	t.set("render_culled", sol::readonly(&RenderManagerDebugInfo::render_culled));
	t.set("depth_culled_list", sol::readonly(&RenderManagerDebugInfo::depth_culled_list));
}

void luawrap_RenderManager(sol::table& m) {
	auto t = m.new_usertype<RenderManager>("RenderManager");
	t[sol::meta_function::construct] = sol::no_constructor;
	t.set_function("add_mesh", &RenderManager::add_mesh);
	t.set_function("add_geometry_mesh", &RenderManager::add_geometry_mesh);
	t.set_function("get_debug_info", &RenderManager::get_debug_info);
}

void luawrap_render_manager(sol::table& m) {
	luawrap_PartSlashTotal(m);
	luawrap_RenderManagerDebugInfo(m);
	luawrap_RenderManager(m);
}
