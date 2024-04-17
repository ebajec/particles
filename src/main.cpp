
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

#define WINDOW_HEIGHT 1280	
#define WINDOW_WIDTH 1280

#define NPARTS 100000

class ParticleSimulation : public BaseViewWindow {
protected:
	/******* THIS STUFF RUNS AFTER OPENGL CONTEXT IS INITIALIZED ******/ 
	void _main() {
		// tell GL to only draw onto a pixel if the shape is closer to the viewer
		glEnable(GL_DEPTH_TEST); // enable depth-testing

		_main_shader = ShaderProgram("../shader/vertex.glsl", "../shader/frag.glsl");
		
		this->parts_ptr = new Particles(NPARTS);

		system = ComputeShader("../shader/systemsecondorder.glsl");

		// Setup Dear ImGui context
    	IMGUI_CHECKVERSION();
    	ImGui::CreateContext();
    	ImGuiIO& io = ImGui::GetIO();

    	// Setup Platform/Renderer bindings
    	ImGui_ImplGlfw_InitForOpenGL(_window, true);
    	ImGui_ImplOpenGL3_Init("#version 130");

    	while (!glfwWindowShouldClose(_window)) {
    	    glfwPollEvents();
    	    glfwGetFramebufferSize(_window, &_width, &_height);
    	    glViewport(0, 0, _width, _height);
			_cam.setScreenRatio(_width,_height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			if (is_running) _updateSystem();
			_renderSystem();
			_renderUI();
    	    glfwSwapBuffers(_window);
    	}

    	// Cleanup
    	ImGui_ImplOpenGL3_Shutdown();
    	ImGui_ImplGlfw_Shutdown();
    	ImGui::DestroyContext();
	}

	void _updateSystem(){
		// Set up compute shader uniforms
		system.use();
		system.setUniform("damp",damp);
		system.setUniform("f",f);
		system.setUniform("a",a);
		system.setUniform("b",b);
		system.setUniform("c",c);
		parts_ptr->update(system,{128,1,1});
	}

	void _renderSystem() {
		// Set up main shader uniforms
		_main_shader.use();
		_cam.connectUniforms(_main_shader);
		parts_ptr->draw(_main_shader);
	}

	void _renderUI(){
		// Start the Dear ImGui frame
    	ImGui_ImplOpenGL3_NewFrame();
    	ImGui_ImplGlfw_NewFrame();
    	ImGui::NewFrame();

    	// Parameters controls
    	ImGui::Begin("Parameters");                          
    	ImGui::SliderFloat("damping", &damp, 0.0f, 1.0f);
		ImGui::SliderFloat("f_r", &f, -50, 50);
		ImGui::SliderFloat("a", &a, 0, 1.0f);
		ImGui::SliderFloat("b", &b, 0, 1.0f);
		ImGui::SliderFloat("c", &c, 0, 1.0f);
    	ImGui::End();

		// Application controls
		ImGui::Begin("Setup Config");
		if (ImGui::Button("Start", ImVec2(100,30))) is_running = true;
		if (ImGui::Button("Stop", ImVec2(100,30))) is_running = false;
		if (ImGui::Button("Reset", ImVec2(100,30))) parts_ptr = new Particles(NPARTS);
		ImGui::End();

		// Camera controls
    	ImGui::Begin("Camera");                          
		ImGui::SliderFloat("speed", &_cam_manager.movespeed, 0, 1.0f);
    	ImGui::End();

    	// Rendering
    	ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());	
	}
	void resetParams() {
		damp = 0;
		f = 0;
		a = 0;
		b = 0;
		c = 0;
	}

	Particles* parts_ptr;
	ComputeShader system;
	float damp; //power decrease at large distances
	float f;//radial acceleration	
	float a; //rotation in xy
	float b; //rotation in yz
	float c; //rotation in xz
	bool is_running = false;
public:
	ParticleSimulation(int width, int height) : BaseViewWindow(width, height){
		resetParams();
	}
	void close() {
		is_running = false;
		resetParams();
		delete parts_ptr;
		BaseViewWindow::close();
	}
};


int main() {
	std::cout << "Welcome.\n\n";

	ParticleSimulation window(WINDOW_WIDTH, WINDOW_HEIGHT);

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
