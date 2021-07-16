#pragma once

#include <vector>

#include "camera.h"


struct Cascades {
	int cascade_count;
	std::vector<float> splits;
	std::vector<Transform> ortho_transforms;
	std::vector<OrthographicData> ortho_datas;
};

Cascades calc_cascades(int cascade_count, Transform camera_transform, PerspectiveData perspective_data, glm::quat orientation);
