#include "Camera.h"
Camera::Camera() {
	lower_left_corner = glm::vec3(-2.0, -1.0, -1.0);
	horizontal = glm::vec3(4.0, 0.0, 0.0);
	vertical = glm::vec3(0.0, 2.0, 0.0);
	origin = glm::vec3(0.0, 0.0, 0.0);
}

ray Camera::get_ray(float u, float v) {
	return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
}