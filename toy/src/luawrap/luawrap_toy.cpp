#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include "luawrap_toy.h"

#include "luawrap_timer_manager.h"
#include "luawrap_input_manager.h"
#include "luawrap_camera.h"
#include "luawrap_camera_manager.h"
#include "luawrap_camera_controller.h"
#include "luawrap_render_manager.h"
#include "luawrap_mesh.h"
#include "luawrap_mesh_data.h"
#include "luawrap_material.h"
#include "luawrap_geometry_builder.h"
#include "luawrap_text_builder.h"
#include "luawrap_texture.h"
#include "luawrap_texture_cubemap.h"
#include "luawrap_resource_manager.h"
#include "luawrap_basic_pipeline.h"
#include "luawrap_geometry_mesh.h"
#include "luawrap_light.h"
#include "luawrap_light_manager.h"
#include "luawrap_window.h"
#include "luawrap_script_runner.h"

#include "luawrap_app.h"

#include "global_app.h"


static void luawrap_toy(sol::table& m) {
	luawrap_timer_manager(m);
	luawrap_input_manager(m);
	luawrap_camera(m);
	luawrap_camera_manager(m);
	luawrap_camera_controller(m);
	luawrap_render_manager(m);
	luawrap_mesh(m);
	luawrap_mesh_data(m);
	luawrap_material(m);
	luawrap_geometry_builder(m);
	luawrap_text_builder(m);
	luawrap_texture(m);
	luawrap_texture_cubemap(m);
	luawrap_resource_manager(m);
	luawrap_basic_pipeline(m);
	luawrap_geometry_mesh(m);
	luawrap_light(m);
	luawrap_light_manager(m);
	luawrap_window(m);
	luawrap_script_runner(m);

	luawrap_app(m);
}

int luaopen_toy(lua_State * L) {
	sol::state_view lua(L);
	sol::table m = lua.create_table();

	luawrap_toy(m);

	m["app"] = g_app;
	m.set_function("get_app", []() {
		return g_app;
	});

	sol::stack::push(lua, m);
	return 1;
}
