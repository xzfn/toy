#include "camera_controller.h"
#include "transform_util.h"
#include "camera.h"


CameraController::CameraController()
{
}

void CameraController::set_controlled_camera(Camera* camera)
{
	m_camera = camera;
}

void CameraController::begin_control(Camera* camera)
{
	glm::mat4 view = camera->get_view();
	glm::mat4 matrix = glm::inverse(view);
	m_transform = mat4_to_rigid_transform(matrix);
}

void CameraController::translate(glm::vec3 movement)
{
	Transform new_transform = move_transform(rigid_transform_to_transform(m_transform), movement);
	m_transform = transform_to_rigid_transform(new_transform);
}

void CameraController::rotate(float horz, float vert)
{
	glm::quat q_horz = glm::angleAxis(horz, VEC3_Y);
	glm::vec3 axis_x = quat_axis_x(m_transform.rotation);
	glm::quat q_vert = glm::angleAxis(vert, axis_x);
	m_transform.rotation = q_horz * q_vert * m_transform.rotation;
}

void CameraController::set_transform(Transform transform)
{
	m_transform = transform_to_rigid_transform(transform);
}

Transform CameraController::get_transform()
{
	return rigid_transform_to_transform(m_transform);
}
