
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

#define NPARTS 100
//#define BALLS

#ifdef BALLS
void set_ball_positions(Mesh** balls, Particles* particles) {
	float* positions; 
	particles->openBuffer(&positions,GL_ARRAY_BUFFER,PART_POS,GL_MAP_READ_BIT);
	for (int i = 0; i < NPARTS; i++) {
		vec4 pos = vec4(positions + 4*i) + vec4{0,0,0,1};
		balls[i]->setModel((mat3::id()|vec3(0.0f)).transpose()|pos);
		//pos.print();std::cout << "\n";
		
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
}
#endif

class ParticleSimulation : public BaseViewWindow {
protected:
	void _main() {
		_main_shader = ShaderProgram("../shader/vertex.glsl", "../shader/frag.glsl");
		ComputeShader gravity_collision = ComputeShader("../shader/systemfirstorder.glsl");

		Particles parts(NPARTS);

		#ifdef BALLS
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
		#endif
		
		//main loop
		glfwSetTime(0);
		while (!glfwWindowShouldClose(_window)) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			_main_shader.use();
			_cam.connectUniforms(_main_shader);
			_main_shader.setUniform("nPoints",NPARTS);

			parts.update(gravity_collision,{64,16,1});

			parts.draw(_main_shader);
			
			#ifdef BALLS
			set_ball_positions(balls, &parts);
			for (int i = 0; i< NPARTS; i++) {
				balls[i]->draw(_main_shader);
			}
			#endif

			glfwSwapBuffers(_window);
			glfwPollEvents();
		}

		#ifdef BALLS
		for (int i = 0; i < NPARTS; i++) {
			balls[i]->~Mesh();
		}
		delete[] balls;
		#endif
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
