#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include "drawable.h"

template<int nVBO, int nSSBO>
void GLBufferWrapper<nVBO,nSSBO>::draw(ShaderProgram shader,int count)
{
	glUseProgram(shader.program);
	shader.setUniform("geom_model", _model, GL_TRUE);
	glBindVertexArray(_vao);
	glDrawArrays(_draw_mode, 0, vPrimitives());
	shader.setUniform("geom_model", mat4::id(), GL_FALSE);
}

template<int nVBO, int nSSBO>
GLBufferWrapper<nVBO, nSSBO>::GLBufferWrapper()
{
	glGenVertexArrays(1, &_vao);

	for (int i = 0; i < nVBO; i++) {
		glGenBuffers(1, _vbos + i);
	}

	for (int i = 0; i < nSSBO; i++) {
		glGenBuffers(1,_ssbo + i);
	}


	_draw_mode = GL_POINTS;
	_model = mat4{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
}

template<int nVBO, int nSSBO>
void GLBufferWrapper<nVBO, nSSBO>::initBuffers(GLenum usage)
{
	float** vbufs = new float*[nVBO];
	float** sbufs = new float*[nSSBO];
	
	for (int i = 0; i < nVBO ; i++) {
		int size = std430BufSize(i);
		vbufs[i] = new float[size];
	}
	for (int i = 0; i < nSSBO; i++) {
		int size = std430BufSize(i);
		sbufs[i] = new float[size];
	}

	_load(vbufs,sbufs);
	glBindVertexArray(_vao);

	//copy data in vbufs to corresponding binding vertex array buffers
	for (int i = 0; i < nVBO ; i++) {
		glEnableVertexAttribArray(i);
		glBindBuffer(GL_ARRAY_BUFFER, _vbos[i]);
		glBufferData(GL_ARRAY_BUFFER, std430BufSize(i) * sizeof(float), vbufs[i], usage);
		glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	}

	for (int i = 0; i < nSSBO; i++) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssbo[i]);
		glBufferData(GL_SHADER_STORAGE_BUFFER, std430BufSize(i) * sizeof(float), sbufs[i], usage);
	}
	return;
}
#endif