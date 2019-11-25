#pragma once
#include "hittable.h"
#include <glm/glm.hpp>
class sphere : public hittable	{
	public: 
		sphere();
		sphere(glm::vec3 cen, float r, Material* m);
		virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec)const;
		glm::vec3 center;
		float radius;
		Material* mat;
};

