#include "luawrap_geometry_mesh.h"

#include "geometry_mesh.h"
#include "load_helper.h"


void luawrap_geometry_mesh(sol::table& m) {
	auto t = m.new_usertype<GeometryMesh>("GeometryMesh");

	t.set_function("create", sol::factories(&create_geometry_mesh));
}
