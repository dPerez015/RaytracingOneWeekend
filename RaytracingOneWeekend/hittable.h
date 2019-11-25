#ifndef  HITTABLEH
#define HITTABLEH 
#include "ray.h"

class Material;

struct  hit_record{
	float t;
	glm::vec3 p;
	glm::vec3 normal;
	Material* mat_ptr;
};

class hittable {
	public:		
		virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
};

#endif // ! HITTABLEH
