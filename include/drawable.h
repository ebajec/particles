#pragma once

#ifndef DRAWABLE_H
#define DRAWABLE_H

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

	void setMode(GLenum mode) { _draw_mode = mode; }

	//Number of individual points in vertex array, i.e, the shaders will
	//run this many times.
	virtual unsigned long vPrimitives() { return 0; }

	virtual unsigned long sPrimitives(int binding) { return 0; }

protected:
	//identifies each binding with location in vertex array.  Only really needed
	//if using the same shader for different types objects.
	int _vbo_layout[nVBO];
	int _ssbo_layout[nSSBO];

	//Size of vPrimitives for each binding, in number of floating point values.  NOT size in bytes.
	//ex: if binding 0 was a 3D position, _primitive_sizes[0] = 1. 
	int _vbo_primitives[nVBO];
	int _ssbo_primitives[nSSBO];

	//transformation applied to vertices in vertex shader
	mat4 _model;

	//specifies first argument in glDrawArrays
	GLenum _draw_mode;

	GLuint _vao;
	GLuint _vbos[nVBO];
	GLuint _ssbo[nSSBO];

	//Size of buffer in number of floating point values.  NOT size in bytes.
	//If the position buffer had 100 points, this would be 100 for.
	//@param binding index of vertex buffer object to be sized
	size_t std430BufSize(int location) { return vPrimitives() * 4; }

	//define layout map and primitive sizes here
	virtual void _init() = 0;

	/*copies attributes into NUM_ATTRIBUTES blocks of memory in attribute_buffers
	each block will be of size bufsize(binding)*/
	virtual void _load(float** vbufs,float** sbufs) = 0;

};



#include "drawable.hpp"

#endif