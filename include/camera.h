#pragma once
#include "matrix.h"
#include "shader.h"
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 

#ifndef CAMERA_H
#define CAMERA_H
#define PI 3.141592654f

using vec3 = matrix<3, 1, GLfloat>;
using vec2 = matrix<2, 1, GLfloat>;
using vec4 = matrix<4, 1, GLfloat>;
using mat3 = matrix<3, 3, GLfloat>;
using mat4 = matrix<4, 4, GLfloat>;

struct Camera {
public:
	Camera() { this->_near_dist = 0; this->_far_dist = 0; }

	Camera(
		vec3 normal,
		vec3 pos,
		int w_screen = 1920,
		int h_screen = 1080,
		GLfloat focus = PI / 3,
		GLfloat far = 500
	);

	void connectUniforms(const ShaderProgram& shader);
	void rotate(float pitch, float yaw);
	/*Moves camera in direction delta. 
	*/
	void translate(vec3 delta);
	void reset();
	vec3 pos() {return  (_pos - basis[2]*_near_dist);}
	vec3 coord(int i){return mat3(_model_pitch * _model_yaw)* basis[i];}
	int _h_screen;
	int _w_screen;

	GLfloat _near_dist;
	GLfloat _far_dist;

	vec3 _pos;
	vec3 basis[3];
	mat3 change_of_basis;
	mat4 _world;
	//vertical rotations
	mat4 _model_pitch;
	//horizontal rotations
	mat4 _model_yaw;
	mat4 _view;
	mat4 _proj;

	void _updateViewMat() {
		_view = mat4(change_of_basis);
		_world = mat4(mat3::id() | -1 * (_pos - basis[2] * _near_dist));
	}
};



#endif
