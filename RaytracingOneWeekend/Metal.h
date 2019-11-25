#pragma once
#include "Material.h"
#include <glm/geometric.hpp>
class Metal : public Material {
public:
	Metal (const glm::vec3& a): albedo(a){}
	virtual bool scatter(const ray& r_in, const hit_record& rec, glm::vec3& attenuation, ray& scattered) const {
		glm::vec3 reflected = glm::reflect(glm::normalize(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected);
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}

	glm::vec3 albedo;
};