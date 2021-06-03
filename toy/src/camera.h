#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

#include "glm_util.h"


struct PerspectiveData {
	float fov;
	float aspect;
	float z_near;
	float z_far;
};

struct OrthographicData {
	float left;
	float right;
	float bottom;
	float top;
	float z_near;
	float z_far;
};

class Camera {
public:
	Camera();

	void set_view(glm::mat4 view);

	void set_view_size(float width, float height);

	void set_perspective_param(float fov, float aspect, float z_near, float z_far);
	void set_perspective_data(PerspectiveData data);
	PerspectiveData get_perspective_data();

	void set_orthographic_param(float left, float right, float bottom, float top, float z_near, float z_far);
	void set_orthographic_data(OrthographicData data);
	OrthographicData get_orthographic_data();

	void set_perspective(bool is_perspective);
	bool is_perspective();

	glm::mat4 get_view();
	glm::mat4 get_projection();
	glm::mat4 get_view_projection();

	glm::vec3 screen_to_world(glm::vec3 point);
	Ray screen_to_world_ray(glm::vec3 point);

	glm::vec3 world_to_screen(glm::vec3 position);

private:
	bool m_is_perspective;
	PerspectiveData m_perspective_data;
	OrthographicData m_orthographic_data;
	glm::mat4 m_view;
	float m_view_width;
	float m_view_height;
};
