#define GLM_ENABLE_EXPERIMENTAL
#include <fstream>
#include <iostream>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/norm.hpp>

#include <Windows.h>
#include <GL\glew.h>
#include <SDL2\SDL.h>
#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "ray.h"
#include "Camera.h"
#include "hittable_list.h"
#include "sphere.h"
#include "SFC.h"
#include "Lambertian.h"
#include "Metal.h"

//Texture rendering shaders
#pragma region SCREEN QUAD

GLuint textureQuadVao;
GLuint textureQuadVbo;

GLuint quadTexture;

GLuint quadProgram;
GLuint QuadShader[2];

float verts[] = {
	 1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
	 1.0f,-1.0f, 0.0f,   1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,   0.0f, 1.0f,
	-1.0f,-1.0f, 0.0f,   0.0f, 0.0f,
};

const char* vertexShader =
"#version 330\n\
\n\
layout (location = 0) in vec3 pos;\n\
layout (location = 1) in vec2 TexCoord;\n\
\n\
out vec2 UV;\n\
void main(){\n\
	gl_Position = vec4(pos, 1.0);\n\
	UV = TexCoord;\n\
}\n\
";

const char* fragmentShader =
"#version 330 \n\
out vec4 FragColor;\n\
in vec2 UV;\n\
\n\
uniform sampler2D myTexture;\n\
\n\
void main()\n\
{\n\
	FragColor = texture(myTexture,UV);\n\
	//FragColor = vec4(UV.x,UV.y,0,0);\n\
}\n\
";
#pragma endregion



//GLFUNCTIONS

GLuint compileShader(const char* shaderStr, GLenum shaderType, const char* name = "") {
	GLuint shader = glCreateShader(shaderType); //Crea un espacio para guardarte el sahder
	glShaderSource(shader, 1, &shaderStr, NULL); //Le pasa el codigo fuente
	glCompileShader(shader);  //Compilalo
	GLint res;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &res);
		char* buff = new char[res];
		glGetShaderInfoLog(shader, res, &res, buff);
		fprintf(stderr, "Error Shader %s: %s", name, buff);
		delete[] buff;
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

void linkProgram(GLuint program) {
	glLinkProgram(program);
	GLint res;
	glGetProgramiv(program, GL_LINK_STATUS, &res);
	if (res == GL_FALSE) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &res);
		char* buff = new char[res];
		glGetProgramInfoLog(program, res, &res, buff);
		fprintf(stderr, "Error Link: %s", buff);
		delete[] buff;
	}
}

void GLinit(int width, int height) {
	glViewport(0, 0, width, height);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClearDepth(1.f);
	glDepthFunc(GL_EQUAL);
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);

	//Init quad to draw de result to the screen
	glGenVertexArrays(1, &textureQuadVao);
	glGenBuffers(1, &textureQuadVbo);

	glBindVertexArray(textureQuadVao);
	glBindBuffer(GL_ARRAY_BUFFER, textureQuadVbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Create shader program
	QuadShader[0] = compileShader(vertexShader, GL_VERTEX_SHADER, "Quad Vertex Shader");
	QuadShader[1] = compileShader(fragmentShader, GL_FRAGMENT_SHADER, "Quad Fragment Shader");

	quadProgram = glCreateProgram();
	glAttachShader(quadProgram, QuadShader[0]);
	glAttachShader(quadProgram, QuadShader[1]);
	linkProgram(quadProgram);
}

void GLResize(int width, int height) {

	glViewport(0, 0, width, height);
}

void GLcleanup() {
	glDeleteBuffers(1, &textureQuadVbo);
	glDeleteVertexArrays(1, &textureQuadVao);
}

void GLrender() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(quadProgram);
	glBindVertexArray(textureQuadVao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, quadTexture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glUseProgram(0);
	glBindVertexArray(0);
}

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

glm::vec3 color(const ray& r, hittable* world, int depth) {
	hit_record rec;
	if (world->hit(r, 0.001f, 10000.f, rec)) {
		ray scattered;
		glm::vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth + 1);
		}
		else {
			return glm::vec3(0, 0, 0);
		}
	}
	else {
		glm::vec3 normDir = glm::normalize(r.direction());
		float t = 0.5f * (normDir.y + 1.0f); //[0,1]
		return (1.0f - t) * glm::vec3(1.0, 1.0, 1.0) + t * glm::vec3(0.5, 0.7, 1.0); //higher ray direction -> less gray scale and more bluish color. LERP
	}
}

void GUI() {
	bool show = true;
	ImGui::Begin("Physics Parameters", &show, 0);
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	
	ImGui::End();

}

int main(int argc, char** argv) {
	//____Window creation and opengl initialization_____
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		SDL_Quit();
		return -1;
	}
	// Create window
	SDL_Window *mainwindow;
	SDL_GLContext maincontext;

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	mainwindow = SDL_CreateWindow("GL_framework", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!mainwindow) { /* Die if creation failed */
		SDL_Log("Couldn't create SDL window: %s", SDL_GetError());
		SDL_Quit();
		return -1;
	}

	/* Create our opengl context and attach it to our window */
	maincontext = SDL_GL_CreateContext(mainwindow);

	// Init GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		SDL_Log("Glew error: %s\n", glewGetErrorString(err));
	}
	SDL_Log("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	// Disable V-Sync
	SDL_GL_SetSwapInterval(0);

	int display_w, display_h;
	SDL_GL_GetDrawableSize(mainwindow, &display_w, &display_h);

	// Init GL
	GLinit(display_w, display_h);

	// Setup ImGui binding
	ImGui_ImplSdlGL3_Init(mainwindow);

	//Init scene
	//resolution
	long nx = display_w;
	long ny = display_h;
	int ns = 100;//num samples in each pixel

	//frustrum
	glm::vec3 lowerLeftCorner(-2.0, -1.0, -1.0);
	glm::vec3 horizontal(4.0, 0.0, 0.0);
	glm::vec3 vertical(0.0, 2.0, 0.0);
	glm::vec3 origin(0.0,0.0,0.0);
	Camera cam;

	//Scene
	hittable* list[4];
	list[0] = new sphere(glm::vec3(0, 0, -1), 0.5, new lambertian(glm::vec3(0.3, 0.3, 0.8)));
	list[1] = new sphere(glm::vec3(0, -100.5, -1), 100, new lambertian(glm::vec3(0.8, 0.8, 0.0)));
	list[2] = new sphere(glm::vec3(1, 0, -1), 0.5, new Metal(glm::vec3(0.8, 0.2, 0.2),0.3));
	list[3] = new sphere(glm::vec3(-1, 0, -1), 0.5, new Metal(glm::vec3(0.8, 0.8, 0.8),0.8));
	hittable* world = new hittable_list(list, 4);
	
	//rng init
	rng.a = 0;
	rng.b = 0;
	rng.c = 0;
	rng.d= 0;

	//output init
//	float* resImg = new float[static_cast<size_t>(nx) * static_cast<size_t>(ny)* 3];
	unsigned char* resImg = new unsigned char[nx * ny * 3];

	std::ofstream file;
	file.open("output.txt");
	file << "P3\n" << nx << " " << ny << "\n255\n";

	//Time
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	

	//Render scene
	
	for (int j = ny -1; j >=0; j--) {
		for (int i = 0; i < nx; i++) {
			float u = (float)i / (float)nx;
			float v = (float)j / (float)ny;
			glm::vec3 col(0.0, 0.0, 0.0);
			
			for (int s = 0; s < ns; s++) {
				float u = float(i + random_double(&rng)) / float(nx);
				float v = float(j + random_double(&rng)) / float(ny);
				ray r = cam.get_ray(u, v);
				col += color(r, world,0);
			}
			col /= float(ns);
			col = glm::vec3(sqrt(col.x), sqrt(col.y), sqrt(col.z));
			int ir = int(255.99 * col.x);
			int ig = int(255.99 * col.y);
			int ib = int(255.99 * col.z);
			file << ir << " " << ig << " " << ib << "\n";

			resImg[(i*3) + (nx * 3 * j) + 0] = (unsigned char)ir;
			resImg[(i*3) + (nx * 3 * j) + 1] = (unsigned char)ig;
			resImg[(i*3) + (nx * 3 * j) + 2] = (unsigned char)ib;

			ib = ig * ir;

		}
	}
	
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
#if _DEBUG
	std::chrono::milliseconds time_span = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	std::cout << "I took " << time_span.count() << " milliseconds" <<std::endl;
#else
	std::chrono::milliseconds time_span = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	std::cout << "I took " << time_span.count() <<" milliseconds" <<std::endl;
#endif
	file.close();

	//create the texture with the rendered image
	//load texture
	int x, y, n;
	unsigned char* data = stbi_load("outputMetalRoughness.jpg", &x, &y, &n, 3);


	glGenTextures(1, &quadTexture);
	glBindTexture(GL_TEXTURE_2D, quadTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nx, ny, 0, GL_RGB, GL_UNSIGNED_BYTE, resImg);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glUseProgram(quadProgram);
	glUniform1i(glGetUniformLocation(quadProgram, "myTexture"), 0);

	//draw to screen
	bool quit_app = false;
	while (!quit_app) {
		SDL_Event eve;
		while (SDL_PollEvent(&eve)) {
			ImGui_ImplSdlGL3_ProcessEvent(&eve);
			switch (eve.type) {
			case SDL_WINDOWEVENT:
				if (eve.window.event == SDL_WINDOWEVENT_RESIZED) {
					GLResize(eve.window.data1, eve.window.data2);
				}
				break;
			case SDL_QUIT:
				quit_app = true;
				break;
			}
		}
		ImGui_ImplSdlGL3_NewFrame(mainwindow);

		ImGuiIO& io = ImGui::GetIO();
		GUI();

		GLrender();
		ImGui::Render();
		SDL_GL_SwapWindow(mainwindow);
		
	}

	ImGui_ImplSdlGL3_Shutdown();
	GLcleanup();

	SDL_GL_DeleteContext(maincontext);
	SDL_DestroyWindow(mainwindow);
	SDL_Quit();
	return 0;
}