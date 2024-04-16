#ifndef BUFWRAPPER_HPP
#define BUFWRAPPER_HPP

#ifndef BUFWRAPPER_H
#include "bufwrapper.h"
#endif

template<int nVBO, int nSSBO>
void GLBufferWrapper<nVBO,nSSBO>::draw(ShaderProgram shader,int count)
{
	glUseProgram(shader.program);
	shader.setUniform("geom_model", _model, GL_TRUE);
	glBindVertexArray(_vao);
	glDrawArrays(_draw_mode, 0, vboBufSize(0));
	shader.setUniform("geom_model", mat4::id(), GL_FALSE);
}

template <int nVBO, int nSSBO>
void GLBufferWrapper<nVBO, nSSBO>::openBuffer( float** out,GLuint buffer_object, int loc, GLuint access)
{
    float* mem;
	GLuint buf;
	size_t bufsize;
	int n = this->arraySize();

	if (loc > nVBO || loc > nSSBO) throw invalid_argument("Buffer location out of range");
	//if (*range[1]*(*range[0]) < 0 || *range[0] > *range[1]) throw invalid_argument("Invalid range");

	if (buffer_object == GL_SHADER_STORAGE_BUFFER) {
		buf = _ssbo[loc];
		bufsize = this->ssboBufSize(loc) * sizeof(float);
	}
	if (buffer_object == GL_ARRAY_BUFFER) {
		buf = _vbos[loc];
		bufsize = this->arraySize()*_vbo_primitives[loc] * sizeof(float);
	}
	else {
		throw invalid_argument("Specific buffer is invalid");
	}

	glBindBuffer(buffer_object, buf);

	*out = (float*)glMapBufferRange(buffer_object, 0, bufsize, access);

	return;
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
		vbufs[i] = new float[vboBufSize(i)];
	}
	for (int i = 0; i < nSSBO; i++) {
		sbufs[i] = new float[ssboBufSize(i)];
	}

	_load(vbufs,sbufs);
	glBindVertexArray(_vao);

	//copy data in vbufs to corresponding binding vertex array buffers
	for (int i = 0; i < nVBO ; i++) {
		glEnableVertexAttribArray(i);
		glBindBuffer(GL_ARRAY_BUFFER, _vbos[i]);
		glBufferData(GL_ARRAY_BUFFER, vboBufSize(i) * sizeof(float), vbufs[i], usage);
		glVertexAttribPointer(_vbo_layout[i], _vbo_primitives[i], GL_FLOAT, GL_FALSE, 0, NULL);
	}

	for (int i = 0; i < nSSBO; i++) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssbo[i]);
		glBufferData(GL_SHADER_STORAGE_BUFFER, ssboBufSize(i) * sizeof(float), sbufs[i], usage);
	}
	return;
}
#endif