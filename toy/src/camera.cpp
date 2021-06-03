#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "camera.h"

Camera::Camera()
{
    set_perspective(true);
    set_perspective_param(glm::radians(60.0f), 1.0f, 0.1f, 1000.0f);
    set_orthographic_param(-10.0f, 10.0f, -10.0f, 10.0f, -1000.0f, 1000.0f);
}

void Camera::set_view(glm::mat4 view)
{
    m_view = view;
}

void Camera::set_view_size(float width, float height)
{
    m_view_width = width;
    m_view_height = height;
    float aspect = width / height;
    m_perspective_data.aspect = aspect;
    m_orthographic_data.left = m_orthographic_data.bottom * aspect;
    m_orthographic_data.right = m_orthographic_data.top * aspect;
}

void Camera::set_perspective_param(float fov, float aspect, float z_near, float z_far)
{
    m_perspective_data.fov = fov;
    m_perspective_data.aspect = aspect;
    m_perspective_data.z_near = z_near;
    m_perspective_data.z_far = z_far;
}

void Camera::set_perspective_data(PerspectiveData data)
{
    m_perspective_data = data;
}

PerspectiveData Camera::get_perspective_data()
{
    return m_perspective_data;
}

void Camera::set_orthographic_param(float left, float right, float bottom, float top, float z_near, float z_far)
{
    m_orthographic_data.left = left;
    m_orthographic_data.right = right;
    m_orthographic_data.bottom = bottom;
    m_orthographic_data.top = top;
    m_orthographic_data.z_near = z_near;
    m_orthographic_data.z_far = z_far;
}

void Camera::set_orthographic_data(OrthographicData data)
{
    m_orthographic_data = data;
}

OrthographicData Camera::get_orthographic_data()
{
    return m_orthographic_data;
}

void Camera::set_perspective(bool is_perspective)
{
    m_is_perspective = is_perspective;
}

bool Camera::is_perspective()
{
    return m_is_perspective;
}

glm::mat4 Camera::get_view()
{
    return m_view;
}

glm::mat4 Camera::get_projection()
{
    glm::mat4 projection;
    if (m_is_perspective) {
        projection = glm::perspective(
            m_perspective_data.fov, m_perspective_data.aspect,
            m_perspective_data.z_near, m_perspective_data.z_far
        );
    }
    else {
        projection = glm::ortho(
            m_orthographic_data.left, m_orthographic_data.right,
            m_orthographic_data.bottom, m_orthographic_data.top,
            m_orthographic_data.z_near, m_orthographic_data.z_far
        );
    }
    return projection;
}

glm::mat4 Camera::get_view_projection()
{
    return get_projection() * get_view();
}

glm::vec3 Camera::screen_to_world(glm::vec3 point)
{
    if (m_is_perspective) {
        glm::mat4 view_projection = get_view_projection();
        float view_width = m_view_width;
        float view_height = m_view_height;
        float screen_x = point.x;
        float screen_y = point.y;
        float clip_x = screen_x * 2.0f / view_width - 1.0f;
        float clip_y = (view_height - screen_y) * 2.0f / view_height - 1.0f;
        glm::vec3 clip_position(clip_x, clip_y, 1.0f);
        glm::mat4 inverse_view_projection = glm::inverse(view_projection);
        glm::vec3 ray_direction = project_point(inverse_view_projection, clip_position);
        ray_direction = glm::normalize(ray_direction);
        return ray_direction;
    }
    else {
        glm::vec3 ray_direction(glm::inverse(get_view())[2]);
        ray_direction = glm::normalize(ray_direction);
        return ray_direction;
    }
}

Ray Camera::screen_to_world_ray(glm::vec3 point)
{
    if (m_is_perspective) {
        glm::vec3 ray_direction = screen_to_world(point);
        glm::vec3 eye_position = glm::inverse(get_view())[3];
        return Ray(eye_position, ray_direction);
    }
    else {
        glm::mat4 view_projection = get_view_projection();
        float view_width = m_view_width;
        float view_height = m_view_height;
        float screen_x = point.x;
        float screen_y = point.y;
        float clip_x = screen_x * 2.0f / view_width - 1.0f;
        float clip_y = (view_height - screen_y) * 2.0f / view_height - 1.0f;
        glm::vec3 clip_position(clip_x, clip_y, 1.0f);
        glm::mat4 inverse_view_projection = glm::inverse(view_projection);
        glm::vec3 eye_position = project_point(inverse_view_projection, clip_position);
        glm::vec3 ray_direction(glm::inverse(get_view())[2]);
        return Ray(eye_position, ray_direction);
    }
}

glm::vec3 Camera::world_to_screen(glm::vec3 position)
{
    glm::mat4 view_projection = get_view_projection();
    glm::vec3 clip_position = project_point(view_projection, position);
    float view_width = m_view_width;
    float view_height = m_view_height;
    float screen_x = (clip_position.x + 1.0f) * view_width / 2.0f;
    float screen_y = view_height - (clip_position.y + 1.0f) * view_height / 2.0f;
    float screen_z = 1.0f - clip_position.z;
    return glm::vec3(screen_x, screen_y, screen_z);
}
