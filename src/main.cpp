
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

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
 
using vec3 = matrix<3, 1, GLfloat>;
using mat3 = matrix<3, 3, GLfloat>;

#define WINDOW_HEIGHT 800	
#define WINDOW_WIDTH 800

#define NPARTS 100000

class ParticleSimulation : public BaseViewWindow {
protected:
	void _main() {
		_main_shader = ShaderProgram("../shader/vertex.glsl", "../shader/frag.glsl");
		ComputeShader system = ComputeShader("../shader/systemsecondorder.glsl");

		Particles parts(NPARTS);
		
		//Mesh sphere(Surface([](float s, float t) {
		//	return vec3{sin(s)*cos(t),sin(s)*sin(t),cos(s)};
		//}, PI, 2*PI),
		//2, 100, 100);
		//sphere.initBuffers(GL_STREAM_DRAW);

		// Setup Dear ImGui context
    	IMGUI_CHECKVERSION();
    	ImGui::CreateContext();
    	ImGuiIO& io = ImGui::GetIO();

    	// Setup Platform/Renderer bindings
    	ImGui_ImplGlfw_InitForOpenGL(_window, true);
    	ImGui_ImplOpenGL3_Init("#version 130");

		float damp = 100;
		float a = 1; 
		float b = 1;
		float c = 1;

    	while (!glfwWindowShouldClose(_window)) {
    	    glfwPollEvents();

    	    // Start the Dear ImGui frame
    	    ImGui_ImplOpenGL3_NewFrame();
    	    ImGui_ImplGlfw_NewFrame();
    	    ImGui::NewFrame();

    	    // Define your UI here
    	    ImGui::Begin("Parameters");                          
    	    ImGui::SliderFloat("damping", &damp, 0.0f, 100.0f);
			ImGui::SliderFloat("a", &a, -1.0f, 1.0f);
			ImGui::SliderFloat("b", &b, -1.0f, 1.0f);
			ImGui::SliderFloat("c", &c, -1.0f, 1.0f);

    	    ImGui::End();

    	    // Rendering
    	    ImGui::Render();
    	    glfwGetFramebufferSize(_window, &_width, &_height);
    	    glViewport(0, 0, _width, _height);
			_cam.setScreenRatio(_width,_height);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Set up shader uniform values
			_main_shader.use();
			_cam.connectUniforms(_main_shader);
			system.use();
			system.setUniform("damp",damp);
			system.setUniform("a",a);
			system.setUniform("b",b);
			system.setUniform("c",c);

			parts.update(system,{128,1,1});

			parts.draw(_main_shader);

    	    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    	    glfwSwapBuffers(_window);
    	}

    	// Cleanup
    	ImGui_ImplOpenGL3_Shutdown();
    	ImGui_ImplGlfw_Shutdown();
    	ImGui::DestroyContext();
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
