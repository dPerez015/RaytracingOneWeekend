#pragma once
#include "glm/vec3.hpp"

class ray
{
public:
	ray();
	ray(const glm::vec3& a, const glm::vec3& b);
	glm::vec3 origin()const;
	glm::vec3 direction() const;
	glm::vec3 point_at_parameter(float t)const;

	glm::vec3 A;
	glm::vec3 B;
};

