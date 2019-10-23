#include <fstream>
#include <iostream>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

#include "ray.h"

float hit_sphere(const glm::vec3& center, float radius, const ray& r) {
	glm::vec3 O_C = r.origin() - center;
	float a = glm::dot(r.direction(), r.direction());
	float b = 2.0f * glm::dot(O_C, r.direction());
	float c = glm::dot(O_C, O_C) - radius * radius;
	float discriminant = b * b - 4.0f * a * c;
	
	if (discriminant < 0) {
		return -1.0f;
	}
	return ((-b - std::sqrt(discriminant)) / (2.0f * a));
}

glm::vec3 color(const ray& r) {
	float t = hit_sphere(glm::vec3(0, 0, -1), 0.5, r);
	if (t > 0.0f) {
		glm::vec3 spaceRelativeToSphere = r.point_at_parameter(t) - glm::vec3(0, 0, -1);
		glm::vec3 normal = glm::normalize(r.point_at_parameter(t) - glm::vec3(0, 0, -1));
		return 0.5f * glm::vec3(normal.x + 1, normal.y + 1, normal.z + 1);
	}

	glm::vec3 normDir = glm::normalize(r.direction());
	t = 0.5f * (normDir.y + 1.0f); //[0,1]
	return (1.0f - t) * glm::vec3(1.0, 1.0, 1.0) + t * glm::vec3(0.5, 0.7, 1.0); //higher ray direction -> less gray scale and more bluish color. LERP
}



int main() {
	int nx = 200;
	int ny = 100;
	glm::vec3 lowerLeftCorner(-2.0, -1.0, -1.0);
	glm::vec3 horizontal(4.0, 0.0, 0.0);
	glm::vec3 vertical(0.0, 2.0, 0.0);
	glm::vec3 origin(0.0,0.0,0.0);


	std::ofstream file;
	file.open("output.txt");
	file << "P3\n" << nx << " " << ny << "\n255\n";
	
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	for (int j = ny - 1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			float u = (float)i / (float)nx;
			float v = (float)j / (float)ny;
			ray r(origin, lowerLeftCorner + u * horizontal + v * vertical);
			glm::vec3 col=color(r);
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