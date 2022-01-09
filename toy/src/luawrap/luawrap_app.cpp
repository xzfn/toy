#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include "luawrap_app.h"

#include "toyapp.h"


void luawrap_app(sol::table& m) {
	
	auto t = m.new_usertype<App>("App");
	t["delta_time"] = &App::delta_time;
	t.set("timed_geometry_builder", sol::readonly(&App::timed_geometry_builder));
	t.set_function("set_should_close", &App::set_should_close);
}
