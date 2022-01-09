#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include "lua_util.h"

#include <iostream>

#include "luawrap/luawrap_vmath.h"
#include "luawrap/luawrap_toy.h"


namespace luautil {

static lua_State* s_L;

void set_global_state(lua_State* L) {
	s_L = L;
}

lua_State* get_global_state() {
	return s_L;
}

void bind_all(lua_State* L) {
	std::cout << "lua bind_all\n";
	sol::state_view lua(L);
	sol::table preload = lua["package"]["preload"];
	preload["vmath"] = luaopen_vmath;
	preload["toy"] = luaopen_toy;
}

void run_script(lua_State* L, std::string_view code) {
	sol::state_view lua(L);
	lua.script(code);
}

}
