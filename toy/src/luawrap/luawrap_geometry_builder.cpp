#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include "luawrap_geometry_builder.h"

#include "geometry_builder.h"


void luawrap_GeometryMeshData(sol::table& m) {
	auto t = m.new_usertype<GeometryMeshData>("GeometryMeshData");
	t[sol::meta_function::construct] = sol::constructors<GeometryMeshData()>();
}

void luawrap_GeometryBuilder(sol::table& m) {
	auto t = m.new_usertype<GeometryBuilder>("GeometryBuilder");
	t[sol::meta_function::construct] = sol::constructors<GeometryBuilder()>();
	t.set_function("add_line", &GeometryBuilder::add_line);
	t.set_function("add_sphere", &GeometryBuilder::add_sphere);
	t.set_function("add_cube", &GeometryBuilder::add_cube);
	t.set_function("add_raw_line", &GeometryBuilder::add_raw_line);
	t.set_function("build_data", &GeometryBuilder::build_data);
}

void luawrap_TimedGeometryBuilder(sol::table& m) {
	auto t = m.new_usertype<TimedGeometryBuilder>("TimedGeometryBuilder");
	t[sol::meta_function::construct] = sol::constructors<TimedGeometryBuilder()>();
	t.set_function("add_line", &TimedGeometryBuilder::add_line);
	t.set_function("add_sphere", &TimedGeometryBuilder::add_sphere);
	t.set_function("add_cube", &TimedGeometryBuilder::add_cube);
	t.set_function("add_raw_line", &TimedGeometryBuilder::add_raw_line);
	t.set_function("build_data", &TimedGeometryBuilder::build_data);
}

void luawrap_geometry_builder(sol::table& m) {
	luawrap_GeometryMeshData(m);
	luawrap_GeometryBuilder(m);
	luawrap_TimedGeometryBuilder(m);
}
