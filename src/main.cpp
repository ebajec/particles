
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

#define NPARTS 30

void set_ball_positions(Mesh** balls, Particles* particles) {
	float* positions; 
	particles->openBuffer(&positions,GL_ARRAY_BUFFER,POS,GL_MAP_READ_BIT);
	for (int i = 0; i < NPARTS; i++) {
		vec4 pos = vec4(positions + 4*i) + vec4{0,0,0,1};
		balls[i]->setModel((mat3::id()|vec3(0.0f)).transpose()|pos);
		//pos.print();std::cout << "\n";
		
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

//This is a small example program where I graph part of the imaginary of a mobius
//transformation on a disk in the complex plane.
class particleSimulation : public BaseViewWindow {
protected:
	void _main() {
		_main_shader = ShaderProgram("../shader/vertex.glsl", "../shader/frag.glsl");
		ComputeShader compute = ComputeShader("../shader/gravity.glsl");


		Particles parts(NPARTS,vec3{3,3,3});

		Mesh* balls[NPARTS];

		for (int i = 0; i < NPARTS; i++) {
			balls[i] = new Mesh(
				Surface([=](float s, float t){
					return Sphere(part_size)(s,t);
				},PI,2*PI),
				2,
				40,
				40);
			//balls[i]->setType(LINE);
			balls[i]->initBuffers(GL_STREAM_DRAW);
			balls[i]->colorCurvature(PI/3*(float)(i+2)/NPARTS);
		} 
		
		
		parts.setMode(GL_LINE_LOOP);
		//main loop
		glfwSetTime(0);
		while (!glfwWindowShouldClose(_window)) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			_main_shader.use();
			_cam.connectUniforms(_main_shader);
			_main_shader.setUniform("nPoints",NPARTS);

			set_ball_positions(balls, &parts);
			parts.update(compute);

			//parts.draw(_main_shader);

			for (int i = 0; i< NPARTS; i++) {
				balls[i]->draw(_main_shader);
			}

			glfwSwapBuffers(_window);
			glfwPollEvents();
		}

		for (int i = 0; i < NPARTS; i++) {
			//balls[i]->~Mesh();
		}
		delete[] balls;
	}

	
public:
	float part_size = 1;//radius of each particle
	particleSimulation(int width, int height) : BaseViewWindow(width, height){}
};


int main() {
	std::cout << "Welcome.\n\n";

	particleSimulation  window(WINDOW_WIDTH, WINDOW_HEIGHT);

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
