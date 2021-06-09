#include "camera_manager.h"
#include "global_app.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>

CameraManager::CameraManager()
{
}

void CameraManager::update(float delta_time)
{
	if (m_is_controlled) {
		update_controller(delta_time);
		Transform camera_transform = m_camera_controller.get_transform();
		glm::mat4 view = transform_to_mat4(transform_inverse(camera_transform));
		m_camera.set_view(view);
	}
}

void CameraManager::update_controller(float delta_time)
{
	InputManager& input_manager = get_app()->input_manager;

	if (input_manager.get_key_down(VK_F9)) {
		m_camera.set_perspective(!m_camera.is_perspective());
	}

	bool is_perspective = m_camera.is_perspective();


	float wheel_delta = input_manager.get_mouse_wheel();
	auto move_delta = input_manager.get_mouse_move();
	float delta_x = move_delta.dx;
	float delta_y = move_delta.dy;
	auto mouse_pos = input_manager.get_mouse_position();
	float mouse_x = mouse_pos.x;
	float mouse_y = mouse_pos.y;
	glm::vec2 view_size = get_app()->get_view_size();
	float view_width = view_size.x;
	float view_height = view_size.y;
	float aspect = view_width / view_height;
	bool is_ctrl_down = input_manager.get_key(VK_CONTROL);

	// keyboard movement
	float axis_x = 0.0f;
	float axis_y = 0.0f;
	float axis_z = 0.0f;

	if (is_perspective) {
		if (input_manager.get_key('W')) {
			axis_z += 1.0;
		}
		if (input_manager.get_key('S')) {
			axis_z -= 1.0;
		}
		if (input_manager.get_key('A')) {
			axis_x += 1.0;
		}
		if (input_manager.get_key('D')) {
			axis_x -= 1.0;
		}
		if (input_manager.get_key('E')) {
			axis_y += 1.0;
		}
		if (input_manager.get_key('Q')) {
			axis_y -= 1.0;
		}
		if (!is_ctrl_down) {
			axis_z += wheel_delta * m_camera_controller_sensitivity.wheel_perspective_zoom;
		}
	}
	else {
		if (input_manager.get_key('W')) {
			axis_y += 1.0;
		}
		if (input_manager.get_key('S')) {
			axis_y -= 1.0;
		}
		if (input_manager.get_key('A')) {
			axis_x += 1.0;
		}
		if (input_manager.get_key('D')) {
			axis_x -= 1.0;
		}
	}
	glm::vec3 input_movement(-axis_x, axis_y, -axis_z);
	float keyboard_move_coeff = m_camera_controller_sensitivity.keyboard_move;
	m_camera_controller.translate(keyboard_move_coeff * delta_time * input_movement);

	// mouse rotate
	float rotate_divide = 300.0f;
	float mouse_rotate_coeff = m_camera_controller_sensitivity.mouse_rotate;
	if (input_manager.get_mouse_button(MouseButtonRight)) {
		m_camera_controller.rotate(-delta_x / rotate_divide * mouse_rotate_coeff, -delta_y / rotate_divide * mouse_rotate_coeff);
	}

	// mouse pan
	float mouse_pan_coeff = m_camera_controller_sensitivity.mouse_pan;
	if (input_manager.get_mouse_button(MouseButtonMiddle)) {
		if (is_perspective) {
			glm::vec3 pan_movement = mouse_pan_coeff * 0.01f * glm::vec3(-delta_x, delta_y, 0.0);
			m_camera_controller.translate(pan_movement);
		}
		else {
			OrthographicData orthographic_data = m_camera.get_orthographic_data();
			float half_extent = orthographic_data.top;
			glm::vec3 pan_movement = half_extent * 2.0f / view_height * glm::vec3(-delta_x, delta_y, 0.0);
			m_camera_controller.translate(pan_movement);
		}
	}

	// wheel zoom when orthographic
	if (!is_perspective && wheel_delta != 0.0f && !is_ctrl_down) {
		OrthographicData orthographic_data = m_camera.get_orthographic_data();
		const float ratio = m_camera_controller_sensitivity.wheel_orthographic_zoom;
		float multiplier = 1.0f;
		if (wheel_delta > 0.0f) {
			multiplier = ratio;
		}
		else {
			multiplier = 1.0f / ratio;
		}
		float half_extent = orthographic_data.top;

		orthographic_data.top *= multiplier;
		orthographic_data.bottom *= multiplier;
		orthographic_data.left = orthographic_data.bottom * aspect;
		orthographic_data.right = orthographic_data.top * aspect;

		float offset_x = mouse_x - view_width / 2.0f;
		float offset_y = mouse_y - view_height / 2.0f;
		glm::vec3 zoom_movement{ offset_x, -offset_y, 0.0f };
		zoom_movement *= (1.0f - multiplier) * half_extent * 2.0f / view_height;
		m_camera.set_orthographic_data(orthographic_data);
		m_camera_controller.translate(zoom_movement);
	}

	// adjust mouse move sensitivity
	if (wheel_delta != 0.0f && is_ctrl_down) {
		float multiplier = 1.0f;
		float sensitivity_change_ratio = 0.9f;
		if (wheel_delta > 0.0f) {
			multiplier = 1 / sensitivity_change_ratio;
		}
		else {
			multiplier = sensitivity_change_ratio;
		}
		m_camera_controller_sensitivity.keyboard_move *= multiplier;
	}

	// quick reposition
	bool have_quick_reposition = false;
	float distance_to_origin = 10.0f;
	glm::vec3 eye;
	glm::quat rotation;
	if (input_manager.get_key_down('I')) {
		// face +x
		eye = glm::vec3(-distance_to_origin, 0.0f, 0.0f);
		rotation = glm::quatLookAt(glm::normalize(-eye), VEC3_Y);
		have_quick_reposition = true;
	}
	else if (input_manager.get_key_down('O')) {
		// face -y
		eye = glm::vec3(0.0f, distance_to_origin, 0.0f);
		rotation = glm::quatLookAt(glm::normalize(-eye), VEC3_Z);
		have_quick_reposition = true;
	}
	else if (input_manager.get_key_down('P')) {
		// face +z
		eye = glm::vec3(0.0f, 0.0f, -distance_to_origin);
		rotation = glm::quatLookAt(glm::normalize(-eye), VEC3_Y);
		have_quick_reposition = true;
	}
	if (have_quick_reposition) {
		if (input_manager.get_key(VK_CONTROL)) {
			// keep position
			eye = m_camera_controller.get_transform().translation;
		}
		auto quick_transform = Transform(
			eye, rotation, VEC3_ONES
		);
		m_camera_controller.set_transform(quick_transform);
	}

	// testing
	if (input_manager.get_key_down('G')) {

		std::cout << "mouse " << mouse_x << " " << mouse_y << "\n";
		glm::vec3 screen_point = m_camera.world_to_screen(VEC3_ZERO);
		std::cout << "screen " << glm::to_string(screen_point) << "\n";

		std::cout << "-----\n";

		glm::vec3 ray_screen(mouse_x, mouse_y, 0.0f);
		Ray ray = m_camera.screen_to_world_ray(ray_screen);
		glm::vec3 eye_pos = ray.position;
		glm::vec3 ray_direction = ray.direction;

		std::cout << "eye " << glm::to_string(eye_pos) << "\n";
		std::cout << "dir " << glm::to_string(ray_direction) << "\n";
		std::cout << "mouse " << mouse_x << " " << mouse_y << "\n";
		glm::vec3 end_pos = eye_pos + ray_direction * 1000.0f;
		get_app()->timed_geometry_builder.add_line(eye_pos, end_pos, COLOR_CYAN, 5.0f);
		std::cout << "end " << glm::to_string(end_pos);
		screen_point = m_camera.world_to_screen(end_pos);
		std::cout << "screen " << glm::to_string(screen_point) << "\n";

	}

	// testing
	if (input_manager.get_mouse_button_down(MouseButtonLeft)) {

		glm::vec3 ray_screen(mouse_x, mouse_y, 0.0f);
		Ray ray = m_camera.screen_to_world_ray(ray_screen);
		glm::vec3 eye_pos = ray.position;
		glm::vec3 ray_direction = ray.direction;
		if (ray_direction.y != 0.0f) {
			float t = -eye_pos.y / ray_direction.y;
			glm::vec3 floor_point = eye_pos + ray_direction * t;
			get_app()->timed_geometry_builder.add_sphere(floor_point, 1.0f, COLOR_CYAN, 5.0f);
		}
	}


}

Camera* CameraManager::get_camera()
{
	return &m_camera;
}

CameraController* CameraManager::get_camera_controller()
{
	return &m_camera_controller;
}

void CameraManager::set_is_controlled(bool controlled)
{
	m_is_controlled = controlled;
	if (m_is_controlled) {
		m_camera_controller.begin_control(&m_camera);
	}
}

bool CameraManager::is_controlled()
{
	return m_is_controlled;
}
