#include "cascaded_shadow_map_util.h"

#include <cassert>
#include <array>
#include <cmath>
#include <algorithm>

#include "glm_util.h"


#define MAX_CASCADES 4


Cascades calc_cascades(int cascade_count, Transform camera_transform, PerspectiveData perspective_data, glm::quat orientation)
{
	assert(cascade_count == 3);

	float percents[MAX_CASCADES + 1];
	percents[0] = 0.0f;
	percents[1] = 0.006f;
	percents[2] = 0.05f;
	percents[3] = 1.0f;

	float fov = perspective_data.fov;
	float aspect = perspective_data.aspect;
	float z_near = perspective_data.z_near;
	float z_far = perspective_data.z_far;
	float t = std::tan(fov / 2.0f);

	Transform reverse_transform;
	reverse_transform.rotation = orientation;
	Transform concat_transform = transform_inverse(reverse_transform) * camera_transform;

	float splits[MAX_CASCADES + 1];
	glm::vec3 split_ranges[MAX_CASCADES + 1][2];

	for (int isplit = 0; isplit < cascade_count + 1; ++isplit) {
		float split = z_near + percents[isplit] * (z_far - z_near);
		splits[isplit] = split;

		float by = split * t;
		float bx = by * aspect;

		std::array<glm::vec3, 4> corners = {
			glm::vec3(-bx, -by, -split),
			glm::vec3(bx, -by, -split),
			glm::vec3(bx, by, -split),
			glm::vec3(-bx, by, -split)
		};

		for (int icorner = 0; icorner < 4; ++icorner) {
			corners[icorner] = transform_point(concat_transform, corners[icorner]);
		}

		auto x_minmax = std::minmax({ corners[0].x, corners[1].x, corners[2].x, corners[3].x });
		auto y_minmax = std::minmax({ corners[0].y, corners[1].y, corners[2].y, corners[3].y });
		auto z_minmax = std::minmax({ corners[0].z, corners[1].z, corners[2].z, corners[3].z });

		float x_min = x_minmax.first;
		float x_max = x_minmax.second;
		float y_min = y_minmax.first;
		float y_max = y_minmax.second;
		float z_min = z_minmax.first;
		float z_max = z_minmax.second;

		split_ranges[isplit][0] = glm::vec3(x_min, y_min, z_min);
		split_ranges[isplit][1] = glm::vec3(x_max, y_max, z_max);
	}

	std::vector<Transform> ortho_transforms(cascade_count);
	std::vector<OrthographicData> ortho_datas(cascade_count);

	for (int isplit = 0; isplit < cascade_count; ++isplit) {
		glm::vec3 range_min_near = split_ranges[isplit][0];
		glm::vec3 range_max_near = split_ranges[isplit][1];
		glm::vec3 range_min_far = split_ranges[isplit + 1][0];
		glm::vec3 range_max_far = split_ranges[isplit + 1][1];
		float x_min = std::min(range_min_near.x, range_min_far.x);
		float y_min = std::min(range_min_near.y, range_min_far.y);
		float z_min = std::min(range_min_near.z, range_min_far.z);
		float x_max = std::max(range_max_near.x, range_max_far.x);
		float y_max = std::max(range_max_near.y, range_max_far.y);
		float z_max = std::max(range_max_near.z, range_max_far.z);

		float ortho_x = (x_min + x_max) / 2.0f;
		float ortho_y = (y_min + y_max) / 2.0f;
		float ortho_z = z_max;

		float abs_left = (x_max - x_min) / 2.0f;
		float abs_bottom = (y_max - y_min) / 2.0f;

		float ortho_left = -abs_left;
		float ortho_right = abs_left;
		float ortho_bottom = -abs_bottom;
		float ortho_top = abs_bottom;
		float ortho_z_near = 0.0f;
		float ortho_z_far = z_max - z_min;
	
		Transform ortho_transform;
		OrthographicData ortho_data;

		ortho_transform.translation = quat_transform_point(orientation, glm::vec3(ortho_x, ortho_y, ortho_z));
		ortho_transform.rotation = orientation;
		ortho_data.left = ortho_left;
		ortho_data.right = ortho_right;
		ortho_data.bottom = ortho_bottom;
		ortho_data.top = ortho_top;
		ortho_data.z_near = ortho_z_near;
		ortho_data.z_far = ortho_z_far;

		ortho_transforms[isplit] = ortho_transform;
		ortho_datas[isplit] = ortho_data;
	}

	Cascades res;
	res.cascade_count = cascade_count;
	res.splits.assign(splits + 1, splits + cascade_count + 1);  // skip near, include far
	res.ortho_transforms = ortho_transforms;
	res.ortho_datas = ortho_datas;
	return res;
}
