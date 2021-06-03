#include "wrap_toy.h"

namespace py = pybind11;

#include "wrap_vmath.h"
#include "wrap_timer_manager.h"
#include "wrap_input_manager.h"
#include "wrap_camera.h"
#include "wrap_camera_manager.h"


void wrap_toy(py::module_& m) {
	wrap_timer_manager(m);
	wrap_input_manager(m);
	wrap_camera(m);
	wrap_camera_manager(m);
}
