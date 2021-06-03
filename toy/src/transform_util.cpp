#include "transform_util.h"

Transform move_transform(Transform transform, glm::vec3 movement)
{
    glm::vec3 world_movement = quat_transform_vector(transform.rotation, movement);
    transform.translation += world_movement;
    return transform;
}
