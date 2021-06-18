#include "toyapp.h"

#include <iostream>

#include <pybind11/embed.h>

#include <pybind11/functional.h>
namespace py = pybind11;

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

	App app;
	g_app = &app;
	std::cout << "startup\n";
	app.startup(vulkan_context, &window);

	std::cout << "mainloop\n";
	window.mainloop();

	std::cout << "shutdown\n";
	app.shutdown();
	return 0;
}