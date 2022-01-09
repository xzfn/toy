#pragma once

#include <lua.hpp>

#include <string_view>


namespace luautil {
void set_global_state(lua_State* L);
lua_State* get_global_state();
void bind_all(lua_State* L);
void run_script(lua_State* L, std::string_view code);

}
