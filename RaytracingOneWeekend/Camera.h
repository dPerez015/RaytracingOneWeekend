#pragma once

#include "ray.h"

class Camera{
public: 
	Camera();
	ray get_ray(float u, float v);

	glm::vec3 origin;
	glm::vec3 lower_left_corner;
	glm::vec3 horizontal;
	glm::vec3 vertical;
};

