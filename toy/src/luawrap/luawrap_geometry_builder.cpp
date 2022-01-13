#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include "luawrap_geometry_builder.h"

#include "geometry_builder.h"


void luawrap_GeometryMeshData(sol::table& m) {
	auto t = m.new_usertype<GeometryMeshData>("GeometryMeshData");
	t[sol::call_constructor] = sol::constructors<GeometryMeshData()>();
}

void luawrap_GeometryBuilder(sol::table& m) {
	auto t = m.new_usertype<GeometryBuilder>("GeometryBuilder");
	t[sol::call_constructor] = sol::constructors<GeometryBuilder()>();
	t.set_function("add_line", &GeometryBuilder::add_line);
}

void luawrap_TimedGeometryBuilder(sol::table& m) {
	auto t = m.new_usertype<TimedGeometryBuilder>("TimedGeometryBuilder");
	t[sol::call_constructor] = sol::constructors<TimedGeometryBuilder()>();
	t.set_function("add_line", &TimedGeometryBuilder::add_line);
}

void luawrap_geometry_builder(sol::table& m) {
	luawrap_GeometryMeshData(m);
	luawrap_GeometryBuilder(m);
	luawrap_TimedGeometryBuilder(m);
}
