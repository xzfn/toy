#include "toyapp.h"

#include <iostream>

#include <lua.hpp>

#include <pybind11/embed.h>

#include <pybind11/functional.h>
namespace py = pybind11;

#include "lua_util.h"


App* g_app;

int main() {


	Window window;

	int width = 600;
	int height = 600;

	window.init(width, height, L"toy");

	HINSTANCE hinstance = window.get_win32_hinstance();
	HWND hwnd = window.get_win32_hwnd();

	VulkanContext vulkan_context(hinstance, hwnd, width, height);

	py::scoped_interpreter guard{};
	py::print("hello world");

	lua_State* L = luaL_newstate();
	luautil::set_global_state(L);
	luaL_openlibs(L);
	luautil::bind_all(L);

	App app;
	g_app = &app;
	std::cout << "startup\n";
	app.startup(vulkan_context, &window);

	std::cout << "mainloop\n";
	window.mainloop();

	std::cout << "shutdown\n";
	app.shutdown();

	lua_close(L);
	return 0;
}
