#pragma once
#include "Material.h"
#include <glm/geometric.hpp>
class Metal : public Material {
public:
	Metal (const glm::vec3& a, float f): albedo(a){
		if (f < 1)fuzz = f;
		else fuzz = 1;
	}
	virtual bool scatter(const ray& r_in, const hit_record& rec, glm::vec3& attenuation, ray& scattered) const {
		glm::vec3 reflected = glm::reflect(glm::normalize(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}

	glm::vec3 albedo;
	float fuzz;
};