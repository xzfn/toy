#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include "luawrap_app.h"

#include "toyapp.h"


void luawrap_app(sol::table& m) {
	
	auto t = m.new_usertype<App>("App");
	t.set("delta_time", sol::readonly(&App::delta_time));

	t.set("timer_manager", sol::readonly(&App::timer_manager));
	t.set("timed_text_builder", sol::readonly(&App::timed_text_builder));
	t.set("timed_geometry_builder", sol::readonly(&App::timed_geometry_builder));
	t.set("input_manager", sol::readonly(&App::input_manager));
	t.set("camera_manager", sol::readonly(&App::camera_manager));
	t.set("resource_manager", sol::readonly(&App::resource_manager));
	t.set("render_manager", sol::readonly(&App::render_manager));
	t.set("light_manager", sol::readonly(&App::light_manager));
	t.set("script_runner", sol::readonly(&App::script_runner));
	t.set("material_lines", sol::readonly(&App::material_lines));

	t.set_function("set_background_color", &App::set_background_color);
	t.set_function("bind_key_down", [](App* this_, sol::protected_function lua_callback) {
		KeyDownCallback callback = [lua_callback](uint32_t key) {
			auto lua_ret = lua_callback(key);
			if (!lua_ret.valid()) {
				sol::error err = lua_ret;
				std::cout << "ERROR lua error " << err.what() << "\n";
			}
		};
		this_->luascript_on_key_down = callback;
	});
	t.set_function("get_window", &App::get_window);

	t.set_function("set_should_close", &App::set_should_close);
}
