#pragma once

#include "camera.h"
#include "camera_controller.h"



class CameraManager {
public:
	CameraManager();
	CameraManager(const CameraManager&) = delete;
	CameraManager& operator=(const CameraManager&) = delete;

public:
	void update(float delta_time);

private:
	void update_controller(float delta_time);

public:
	Camera* get_camera();
	CameraController* get_camera_controller();
	void set_is_controlled(bool controlled);
	bool is_controlled();

private:
	bool m_is_controlled{ true };
	Camera m_camera;
	CameraController m_camera_controller;
};
