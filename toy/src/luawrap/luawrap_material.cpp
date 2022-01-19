#include "luawrap_material.h"

#include "material.h"
#include "load_helper.h"


void luawrap_material(sol::table& m) {
	auto t = m.new_usertype<Material>("Material");

	t.set_function("create", sol::factories(&create_material));
	t.set_function("set_base_color", [](Material* this_, glm::vec3 color) {
		MaterialUniforms& uniforms = this_->ref_uniforms();
		uniforms.base_color = color;
	});
}
