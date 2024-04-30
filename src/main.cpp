
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

	void _main(); // Anything which needs to run after OpenGL context is initialized
				  // must go here.
	void _updateSystem();
	void _renderSystem();
	void _renderUI();
	void _resetParams();
	void _buttonReset();
	void _buttonStart();
	void _buttonStop();
	bool is_running = false;
	Particles* parts_ptr;
	ShaderProgram main_program;
	ShaderProgram system;
	float stop_time;
	float damp;
	float f;
	float a;
	float b;
	float c;
public:
	ParticleSimulation(int width, int height) : BaseViewWindow(width, height){
		_resetParams();
	}
	void close();
};

void ParticleSimulation::_main() {
		// tell GL to only draw onto a pixel if the shape is closer to the viewer
		glEnable(GL_DEPTH_TEST); // enable depth-testing
		main_program = ShaderProgram();
		main_program.addShader("../shader/vertex.glsl", GL_VERTEX_SHADER);
		main_program.addShader("../shader/frag.glsl", GL_FRAGMENT_SHADER);
		main_program.link();
		system = ShaderProgram("../shader/systemsecondorder.glsl", GL_COMPUTE_SHADER);
		
		this->parts_ptr = new Particles(NPARTS);

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
void ParticleSimulation::_updateSystem(){
	// Set up compute shader uniforms
	system.use();
	system.setUniform("damp",damp);
	system.setUniform("f",f);
	system.setUniform("a",a);
	system.setUniform("b",b);
	system.setUniform("c",c);
	parts_ptr->update(system,{128,1,1});
}
void ParticleSimulation::_renderSystem() {
	// Set up main shader uniforms
	main_program.use();
	_cam.connectUniforms(main_program);
	parts_ptr->draw(main_program);
}
void ParticleSimulation::_renderUI(){
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	// Parameters controls
	ImGui::Begin("Parameters");                          
	ImGui::SliderFloat("damping", &damp, 0.0f, 1.0f);
	ImGui::SliderFloat("f_r", &f, -50, 50);
	ImGui::SliderFloat("a", &a, -1.0f, 1.0f);
	ImGui::SliderFloat("b", &b, -1.0f, 1.0f);
	ImGui::SliderFloat("c", &c, -1.0f, 1.0f);
	ImGui::End();
	// Application controls
	ImGui::Begin("Setup Config");
	if (ImGui::Button("Start", ImVec2(100,30))) _buttonStart();
	if (ImGui::Button("Stop", ImVec2(100,30))) _buttonStop();
	if (ImGui::Button("Reset", ImVec2(100,30))) _buttonReset();
	ImGui::End();
	// Camera controls
	ImGui::Begin("Camera");                          
	ImGui::SliderFloat("speed", &_cam_manager.movespeed, 0, 1.0f);
	ImGui::End();
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());	
}
void ParticleSimulation::_resetParams() {
	damp = 0;
	f = 0;
	a = 0;
	b = 0;
	c = 0;
}
void ParticleSimulation::_buttonReset(){
	delete this->parts_ptr;
	this->parts_ptr = new Particles(NPARTS);
}
void ParticleSimulation::_buttonStart(){
	is_running = true;
}
void ParticleSimulation::_buttonStop(){
	is_running = false;
}
void ParticleSimulation::close() {
	is_running = false;
	_resetParams();
	delete parts_ptr;
	BaseViewWindow::close();
}

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
