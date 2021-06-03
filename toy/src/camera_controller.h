#pragma once

#include "glm_util.h"

class Camera;


class CameraController {
public:
	CameraController();

	void set_controlled_camera(Camera* camera);
	void begin_control(Camera* camera);
	void translate(glm::vec3 movement);
	void rotate(float horz, float vert);

	void set_transform(Transform transform);
	Transform get_transform();

private:
	Camera* m_camera{ nullptr };
	RigidTransform m_transform;
};
