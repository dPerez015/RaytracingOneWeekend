#define GLM_ENABLE_EXPERIMENTAL
#include <fstream>
#include <iostream>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/norm.hpp>

#include "ray.h"
#include "Camera.h"
#include "hittable_list.h"
#include "sphere.h"
#include "SFC.h"

sfc rng;

inline double random_double(sfc* d) {
	uint64_t r = (*d)();
	return r / (double)(d->MAXRAND);
}

glm::vec3 random_in_unit_sphere() {
	glm::vec3 p;
	do {
		p = 2.0f * glm::vec3(random_double(&rng), random_double(&rng), random_double(&rng)) - glm::vec3(1.f,1.f,1.f);
	} while (glm::length2(p) >= 1.0f);
	return p;
}

glm::vec3 color(const ray& r, hittable* world) {
	hit_record rec;
	if (world->hit(r, 0.0f, 10000.f, rec)) {
		glm::vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		return 0.5f * color(ray(rec.p, target - rec.p),world);
	}
	else {
		glm::vec3 normDir = glm::normalize(r.direction());
		float t = 0.5f * (normDir.y + 1.0f); //[0,1]
		return (1.0f - t) * glm::vec3(1.0, 1.0, 1.0) + t * glm::vec3(0.5, 0.7, 1.0); //higher ray direction -> less gray scale and more bluish color. LERP
	}
}


int main() {
	int nx = 200;
	int ny = 100;
	int ns = 100;//num samples in each pixel

	glm::vec3 lowerLeftCorner(-2.0, -1.0, -1.0);
	glm::vec3 horizontal(4.0, 0.0, 0.0);
	glm::vec3 vertical(0.0, 2.0, 0.0);
	glm::vec3 origin(0.0,0.0,0.0);
	Camera cam;

	hittable* list[2];
	list[0] = new sphere(glm::vec3(0, 0, -1), 0.5);
	list[1] = new sphere(glm::vec3(0, -100.5, -1), 100);
	hittable* world = new hittable_list(list, 2);

	rng.a = 0;
	rng.b = 0;
	rng.c = 0;
	rng.d= 0;

	std::ofstream file;
	file.open("output.txt");
	file << "P3\n" << nx << " " << ny << "\n255\n";
	
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	for (int j = ny - 1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			float u = (float)i / (float)nx;
			float v = (float)j / (float)ny;
			glm::vec3 col(0.0, 0.0, 0.0);
			
			for (int s = 0; s < ns; s++) {
				float u = float(i + random_double(&rng)) / float(nx);
				float v = float(j + random_double(&rng)) / float(ny);
				ray r = cam.get_ray(u, v);
				col += color(r, world);
			}
			col /= float(ns);
			col = glm::vec3(sqrt(col.x), sqrt(col.y), sqrt(col.z));
			int ir = int(255.99 * col.x);
			int ig = int(255.99 * col.y);
			int ib = int(255.99 * col.z);
			file << ir << " " << ig << " " << ib << "\n";
		}
	}

	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
#if _DEBUG
	std::chrono::milliseconds time_span = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	std::cout << "I took " << time_span.count() << " milliseconds" <<std::endl;
#else
	std::chrono::nanoseconds time_span = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
	std::cout << "I took " << time_span.count() <<" nanoseconds" <<std::endl;
#endif
	file.close();
}