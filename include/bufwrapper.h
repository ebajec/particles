#pragma once

#ifndef BUFWRAPPER_H
#define BUFWRAPPER_H

#include "GL/glew.h"
#include "shader.h"
#include "matrix.h"
#include <type_traits>

template<int nVBO, int nSSBO = 0>
class GLBufferWrapper {
public:
	GLBufferWrapper();
  
	/*Copies data for each vertex binding into GPU buffers.*/
	void initBuffers(GLenum usage);

    // Applies an affine transformation to the object by multiplying the model matrix by A.  
	void transformAffine(mat4 A) { _model = _model * A; }
	void resetTransformations() {_model = mat4(mat3::id());}
	void draw(ShaderProgram shader, int count = -1);
	void setModel(mat4 A) {_model = A;}

	void setMode(GLenum mode) { _draw_mode = mode; }

	// Open buffer for reading by CPU. type must be either "vbo" or "ssbo".  loc
	// is the attribute index.  
	// IMPORTANT: glUnmapBuffer(buffer object) MUST be called afterwards
	void openBuffer(float** out, GLuint buffer_object, int loc, GLuint access);

	// @return Number of points in vertex array
	size_t arraySize() { return _array_size; }

	//Size of buffer in number of floating point values. 
	//@param location index of vertex buffer object to be sized
	size_t vboBufSize(int location) { return _array_size * _vbo_primitives[location]; }
	size_t ssboBufSize(int location) { return _ssbo_sizes[location] * _ssbo_primitives[location];}

protected:
	//identifies each binding with location in vertex array.  Only really needed
	//if using the same shader for different types objects.
	int _vbo_layout[nVBO];
	int _ssbo_layout[nSSBO];

	int _vbo_primitives[nVBO];
	int _ssbo_primitives[nSSBO];

	// Size of vertex array in number of objects
	size_t _array_size;

	// Size of each SSBO in number of objects, i.e., (bytes)/(primtive_size)
	size_t _ssbo_sizes[nSSBO];

	//transformation applied to vertices in vertex shader
	mat4 _model;

	//specifies first argument in glDrawArrays
	GLenum _draw_mode;

	GLuint _vao;
	GLuint _vbos[nVBO];
	GLuint _ssbo[nSSBO];

	

	//define layout map and primitive sizes here
	virtual void _init() = 0;

	/*copies attributes into NUM_ATTRIBUTES blocks of memory in attribute_buffers
	each block will be of size bufsize(binding)*/
	virtual void _load(float** vbufs,float** sbufs) = 0;

};

#include "bufwrapper.hpp"

#endif