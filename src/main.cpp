
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
 
using vec3 = matrix<3, 1, GLfloat>;
using mat3 = matrix<3, 3, GLfloat>;

#define WINDOW_HEIGHT 1080	
#define WINDOW_WIDTH 1920


//This is a small example program where I graph part of the imaginary of a mobius
//transformation on a disk in the complex plane.
class programWindow : public BaseViewWindow {
protected:
	void _main() {
		_main_shader = ShaderProgram("../shader/vertex.glsl", "../shader/frag.glsl");
		ComputeShader compute = ComputeShader("../shader/gravity.glsl");
	
		Particles parts(2,vec3{5,5,5});
		parts.setMode(GL_LINE_LOOP);
		//main loop
		glfwSetTime(0);
		while (!glfwWindowShouldClose(_window)) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			_main_shader.use();
			_cam.connectUniforms(_main_shader);

			parts.update(compute);
			parts.draw(_main_shader);

			glfwSwapBuffers(_window);
			glfwPollEvents();
		}
	}

	
public:
	programWindow(int width, int height) : BaseViewWindow(width, height){}
};


int main() {
	std::cout << "Welcome.\n\n";

	programWindow  window(WINDOW_WIDTH, WINDOW_HEIGHT);

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
