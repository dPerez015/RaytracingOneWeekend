
#include <fstream>
#include <iostream>
#include "glm/vec3.hpp"

int main() {
	int nx = 200;
	int ny = 100;
	std::ofstream file;
	file.open("output.txt");
	file << "P3\n" << nx << " " << ny << "\n255\n";
	for (int j = ny - 1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			glm::vec3 col(float(i) / float(nx), float(j) / float(ny), 0.2);
			int ir = int(255.99 * col.x);
			int ig = int(255.99 * col.y);
			int ib = int(255.99 * col.z);
			file << ir << " " << ig << " " << ib << "\n";
		}
	}
	file.close();
}