#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include "shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#define MAX_LINE_LENGTH 100

inline string read_text_file(const char* src){
	ifstream file(src);

	if (!file.is_open()) {
		fprintf(stderr, "ERROR: could not open file:\n",src,"\n");
		return "";
	}

	string line;
	string text;

	while (std::getline(file,line)) {
		text.append(line).append("\n");
	}

	return text;
}


ShaderProgram::ShaderProgram(
const char* vertex_shader_path, 
const char* fragment_shader_path, 
const char* geometry_shader_path) {
	init();

	string vertex_shader_source = read_text_file(vertex_shader_path);
	string fragment_shader_source = read_text_file(fragment_shader_path);
	string geometry_shader_source = "";

	if (geometry_shader_path != "") {
		//geometry_shader_source = read_text_file(geometry_shader_path);
		//_compileShader(GL_GEOMETRY_SHADER, geometry_shader_source.c_str());
	}

	if (fragment_shader_source == "" || vertex_shader_source == "") {
		fprintf(stderr, "ERROR: could not open shader files\n");
		glfwTerminate();
	}
	
	//compile shaders
	_compileShader(GL_VERTEX_SHADER, vertex_shader_source.c_str());
	_compileShader(GL_FRAGMENT_SHADER, fragment_shader_source.c_str());

	glLinkProgram(program);
}

ComputeShader::ComputeShader(const char* shader_path) {
	init();

	string shader_source = read_text_file(shader_path);

	if (shader_source == "") {
		fprintf(stderr, "ERROR: could not open shader files\n");
		glfwTerminate();
	}

	_compileShader(GL_COMPUTE_SHADER, shader_source.c_str());

	glLinkProgram(program);
}

void ShaderProgram::setUniform(const char* name, int value)
{
	glUniform1i(this->getUniform(name), value);
}

void ShaderProgram::setUniform(const char* name, unsigned int value) {
	glUniform1ui(this->getUniform(name), value);
}

void ShaderProgram::setUniform(const char* name, float value)
{
	glUniform1f(this->getUniform(name), value);
}

void ShaderProgram::setUniform(const char* name, vec3 value)
{
	glUniform3fv(this->getUniform(name), 1, value.data());
}

void ShaderProgram::setUniform(const char* name, mat3 value, GLboolean transpose)
{
	glUniformMatrix3fv(this->getUniform(name), 1, transpose, value.data());
}

void ShaderProgram::setUniform(const char* name, mat4 value, GLboolean transpose)
{
	glUniformMatrix4fv(this->getUniform(name), 1, transpose, value.data());
}

void ShaderProgram::_compileShader(GLenum type, const char* source)
{
	GLuint s = glCreateShader(type);
	glShaderSource(s, 1, &source, NULL);
	glCompileShader(s);

	GLint success = 0;
	glGetShaderiv(s, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		GLint length = 0;
		glGetShaderiv(s, GL_INFO_LOG_LENGTH, &length);

		std::vector<GLchar> error_message(length);
		glGetShaderInfoLog(s, length, NULL, &error_message[0]);

		printf(&error_message[0]);

		glDeleteShader(s);
	}
	else {
		glAttachShader(program, s);
	}


}

