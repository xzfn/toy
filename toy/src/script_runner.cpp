#include "script_runner.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <pybind11/eval.h>
namespace py = pybind11;

#include "lua_util.h"


void ScriptRunner::lua_script(std::string code) {
	sol::state_view lua(luautil::get_global_state());
	lua.script(code);
}

void ScriptRunner::py_script(std::string code) {
	py::exec(code);
}
