
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "matrix.h"
#include "view_window.h"
#include "math.h"
#include "particle.h"
#include "mesh.h"
 
using vec3 = matrix<3, 1, GLfloat>;
using mat3 = matrix<3, 3, GLfloat>;

#define WINDOW_HEIGHT 1080	
#define WINDOW_WIDTH 1920

#define NPARTS 4000

class ParticleSimulation : public BaseViewWindow {
protected:
	void _main() {
		_main_shader = ShaderProgram("../shader/vertex.glsl", "../shader/frag.glsl");
		ComputeShader system = ComputeShader("../shader/systemfirstorder.glsl");

		Particles parts(NPARTS);

		Mesh donut(Surface([=](float s, float t){
			return Torus(3.0f,1.0f)(s,t);
		},2*PI,2*PI),0,50,50);
		donut.checkChar();
		donut.initBuffers(GL_STREAM_DRAW);
		

		//main loop
		glfwSetTime(0);
		while (!glfwWindowShouldClose(_window)) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			_main_shader.use();
			_cam.connectUniforms(_main_shader);

			parts.update(system,{128,1,1});

			parts.draw(_main_shader);
			donut.draw(_main_shader);
			donut.transformAffine(mat4{
				1,0,0,0,
				0,1,0,0,
				0,0,1,0.1,
				0,0,0,1
				});

			glfwSwapBuffers(_window);
			glfwPollEvents();
		}
	}

	
public:
	float part_size = 1;//radius of each particle
	ParticleSimulation(int width, int height) : BaseViewWindow(width, height){}
};



int main() {
	std::cout << "Welcome.\n\n";

	ParticleSimulation  window(WINDOW_WIDTH, WINDOW_HEIGHT);

	std::string command;

	while (true) {
	
		std::cin >> command;

		if (command == "exit") {
			window.close();
			return 0;
		}

		else if (command == "launch" && !window.isRunning()) window.launch("test", NULL, NULL);

		else if (command == "close" && window.isRunning()) window.close();

		else std::cout << "invalid command\n";

	}

	return 0;
}
