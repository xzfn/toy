#include "wrap_toy.h"

namespace py = pybind11;

#include "wrap_vmath.h"
#include "wrap_timer_manager.h"
#include "wrap_input_manager.h"
#include "wrap_camera.h"
#include "wrap_camera_manager.h"
#include "wrap_render_manager.h"
#include "wrap_mesh.h"
#include "wrap_material.h"
#include "wrap_geometry_builder.h"
#include "wrap_text_builder.h"
#include "wrap_texture.h"
#include "wrap_texture_cubemap.h"
#include "wrap_resource_manager.h"
#include "wrap_basic_pipeline.h"
#include "wrap_mesh_data.h"
#include "wrap_geometry_mesh.h"
#include "wrap_light.h"
#include "wrap_light_manager.h"


#include "wrap_app.h"


void wrap_toy(py::module_& m) {
	wrap_timer_manager(m);
	wrap_input_manager(m);
	wrap_camera(m);
	wrap_camera_manager(m);
	wrap_render_manager(m);
	wrap_mesh(m);
	wrap_material(m);
	wrap_geometry_builder(m);
	wrap_text_builder(m);
	wrap_texture(m);
	wrap_texture_cubemap(m);
	wrap_resource_manager(m);
	wrap_basic_pipeline(m);
	wrap_mesh_data(m);
	wrap_geometry_mesh(m);
	wrap_light(m);
	wrap_light_manager(m);

	wrap_app(m);
}
