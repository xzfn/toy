#include "luawrap_basic_pipeline.h"

#include "basic_pipeline.h"
#include "load_helper.h"


void luawrap_basic_pipeline(sol::table& m) {
	auto t = m.new_usertype<BasicPipeline>("BasicPipeline");

	t.set_function("create", sol::factories(&create_basic_pipeline));
	t.set_function("get_shader_spvs", &BasicPipeline::get_shader_spvs);
	t.set_function("reload_shader", &BasicPipeline::reload_shader);
	// t.def("init_resource", &BasicPipeline::init_resource);
}
