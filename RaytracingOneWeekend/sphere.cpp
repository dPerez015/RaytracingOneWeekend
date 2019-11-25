#include "sphere.h"
sphere::sphere() {}

sphere::sphere(glm::vec3 cen, float r) : center(cen), radius(r) {}

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec)const {
	glm::vec3 oc = r.origin() - center;
	float a = glm::dot(r.direction(), r.direction());
	float b = glm::dot(oc, r.direction());
	float c = glm::dot(oc, oc) - radius * radius;
	float discriminant = b * b - a * c; //b^2 = 4* something, so we can divide everything by 4
	
	//if the ray hits we check if any of the solutions are inside the (tmin,tmax) range, prioritizing the smallest t
	if (discriminant > 0) {
		float temp = (-b - sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			return true;
		}
		temp = (-b + sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			return true;
		}
	}
	return false;
}