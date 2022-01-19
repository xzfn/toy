#include "luawrap_resource_manager.h"

#include "resource_manager.h"


void luawrap_resource_manager(sol::table& m) {
	auto t = m.new_usertype<ResourceManager>("ResourceManager");

	t.set_function("get_resource_directories", &ResourceManager::get_resource_directories);
	t.set_function("set_resource_directories", &ResourceManager::set_resource_directories);
	t.set_function("full_path", &ResourceManager::full_path);
	t.set_function("exists", &ResourceManager::exists);
	t.set_function("read_binary",  &ResourceManager::read_binary);
	t.set_function("read_text", &ResourceManager::read_text);
}
