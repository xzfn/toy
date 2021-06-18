
set(toy_SOURCES
	src/vulkan_util.h
	src/vulkan_util.cpp
	src/vulkan_resources.h
	src/vulkan_resources.cpp
	src/vulkan_context.h
	src/vulkan_context.cpp
	src/window.h
	src/window.cpp
	src/file_util.h
	src/file_util.cpp
	src/image_util.h
	src/image_util.cpp
	src/mesh_data.h
	src/mesh_data.cpp
	src/mesh.h
	src/mesh.cpp
	src/material.h
	src/material.cpp
	src/texture.h
	src/texture.cpp
	src/texture_cubemap.h
	src/texture_cubemap.cpp
	src/stl_util.h
	src/camera.h
	src/camera.cpp
	src/glm_util.h
	src/glm_util.cpp
	src/basic_shapes.h
	src/basic_shapes.cpp
	src/basic_pipeline.h
	src/basic_pipeline.cpp
	src/vertex_format.h
	src/vertex_format.cpp
	src/pipeline_util.h
	src/pipeline_util.cpp
	src/volk_impl.cpp
	src/stb_impl.cpp

	src/resource_manager.h
	src/resource_manager.cpp

	src/transform_util.h
	src/transform_util.cpp

	src/color_util.h
	src/color_util.cpp

	src/timed_info_fwd.h
	src/timed_info.h
	src/timed_info.cpp

	src/geometry_builder.h
	src/geometry_builder.cpp
	src/geometry_util.h
	src/geometry_util.cpp
	src/geometry_mesh.h
	src/geometry_mesh.cpp

	src/text_builder.h
	src/text_builder.cpp
	src/ascii_texture.h
	src/ascii_texture.cpp

	src/vertex_mesh.h
	src/vertex_mesh.cpp

	src/camera_controller.h
	src/camera_controller.cpp

	src/camera_manager.h
	src/camera_manager.cpp

	src/timer_manager.h
	src/timer_manager.cpp

	src/input_manager.h
	src/input_manager.cpp

	src/system.h
	src/system.cpp

	src/render_manager.h
	src/render_manager.cpp

	src/scope_util.h
)

set(toy_INCLUDE_DIR src)

add_library(toy STATIC ${toy_SOURCES})

target_compile_definitions(toy PUBLIC ${VULKAN_PLATFORM_DEFINES} ${WIN32_UNICODE_DEFINES})
target_link_libraries(toy PUBLIC volk::volk_headers)
target_link_libraries(toy PUBLIC glm::glm)
target_include_directories(toy PRIVATE ${stb_INCLUDE_DIR})
target_include_directories(toy PUBLIC ${toy_INCLUDE_DIR})

set(toymain_SOURCES
    src/main.cpp
)


add_executable(toymain ${toymain_SOURCES})
target_link_libraries(toymain PUBLIC toy)


set(toyapp_SOURCES
	src/main_toyapp.cpp

	src/toyapp.h
	src/toyapp.cpp

	src/global_app.h
	src/global_app.cpp

	src/load_helper.h
	src/load_helper.cpp

	src/wrap/wrap_toy.h
	src/wrap/wrap_toy.cpp

	src/wrap/wrap_vmath.h
	src/wrap/wrap_vmath.cpp

	src/wrap/wrap_timer_manager.h
	src/wrap/wrap_timer_manager.cpp

	src/wrap/wrap_input_manager.h
	src/wrap/wrap_input_manager.cpp

	src/wrap/wrap_camera.h
	src/wrap/wrap_camera.cpp

	src/wrap/wrap_camera_manager.h
	src/wrap/wrap_camera_manager.cpp

	src/wrap/wrap_render_manager.h
	src/wrap/wrap_render_manager.cpp

	src/wrap/wrap_geometry_builder.h
	src/wrap/wrap_geometry_builder.cpp

	src/wrap/wrap_resource_manager.h
	src/wrap/wrap_resource_manager.cpp

	src/wrap/wrap_text_builder.h
	src/wrap/wrap_text_builder.cpp

	src/wrap/wrap_mesh_data.h
	src/wrap/wrap_mesh_data.cpp

	src/wrap/wrap_mesh.h
	src/wrap/wrap_mesh.cpp

	src/wrap/wrap_material.h
	src/wrap/wrap_material.cpp

	src/wrap/wrap_texture.h
	src/wrap/wrap_texture.cpp

	src/wrap/wrap_texture_cubemap.h
	src/wrap/wrap_texture_cubemap.cpp

	src/wrap/wrap_basic_pipeline.h
	src/wrap/wrap_basic_pipeline.cpp

	src/wrap/wrap_app.h
	src/wrap/wrap_app.cpp

	src/python_util.h
	src/python_util.cpp
)

add_executable(toyapp ${toyapp_SOURCES})
target_link_libraries(toyapp PUBLIC toy)
target_link_libraries(toyapp PRIVATE pybind11::embed)
