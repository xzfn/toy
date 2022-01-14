#include "luawrap_script_runner.h"

#include "script_runner.h"


void luawrap_script_runner(sol::table& m) {
	auto t = m.new_usertype<ScriptRunner>("ScriptRunner");
	t[sol::call_constructor] = sol::no_constructor;
	t.set_function("lua_script", &ScriptRunner::lua_script);
	t.set_function("py_script", &ScriptRunner::py_script);
}
