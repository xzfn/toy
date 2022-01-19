#include "luawrap_mesh.h"

#include "mesh.h"
#include "load_helper.h"


void luawrap_mesh(sol::table& m) {
	auto t = m.new_usertype<Mesh>("Mesh");

	t.set_function("create", sol::factories(&create_mesh));
}
