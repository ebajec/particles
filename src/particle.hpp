#ifndef PARTICLE_HPP
#define PARTICLE_HPP
#include "particle.h"
#include "shapes.h"

void Particles::_init() {
	//define primitive sizes for each vertex buffer and create empty array for each one
	_vbo_primitives[POS] = 3;
	_vbo_primitives[COL] = 3;
	_vbo_layout[POS] = 0;
	_vbo_layout[COL] = 1;
	
	
	_ssbo_primitives[VEL] = 3;
	_ssbo_primitives[ACC] = 3;
	_ssbo_primitives[COLLISION] = 1;
	_ssbo_layout[VEL] = 0;
	_ssbo_layout[ACC] = 1;
	_ssbo_layout[ACC] = 2;
}

Particles::Particles(int count,vec3 bound) : GLBufferWrapper<PART_VBOS,PART_SSBOS>(){
	_init();
	this->_count = count;
	this->_bound = bound;

	this->initBuffers(GL_STREAM_DRAW);

	//set up for compute shader
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,_vbos[POS]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,_ssbo[VEL]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,2,_ssbo[ACC]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,3,_vbos[COL]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,4,_vbos[COLLISION]);
	
}

void Particles::update(ComputeShader shader)
{
	double t = glfwGetTime();

	shader.use();
	shader.setUniform("nPoints", (int)_count);
	shader.setUniform("delta",(float)(t - t_o) );
	shader.setUniform("t", (float)(t));

	glDispatchCompute((int)(_count+63) / 64, (int)(_count+15) / 16, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

	float* mem;
	glBindBuffer(GL_ARRAY_BUFFER, (_vbos)[COL]);
	mem = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, vPrimitives() * sizeof(float), GL_MAP_WRITE_BIT);
	for (int i = 0; i < vPrimitives(); i++) {
		mem[4*i + 3 ] = 0;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	return;
}


vec3 random_in_bounds(vec3 bound) {
	float x = *bound[0]*uniform_rand();
	float y = *bound[1]*uniform_rand();
	float z = *bound[2]*uniform_rand();
	return vec3{x,y,z};
}


inline vec3 point_ball(float r) {
	return Sphere(r)(gaussian(PI/2,PI/5),gaussian(PI,PI));
}

inline vec3 i_torus(float rhole,float rtube, int i,int n) {
	float s = (PI*i)/n;
	float t = (2*PI*(i%(10)))/10;
	return Torus(rhole,rtube)(s,t);
}

inline vec3 path(int i,int n) {
	float t = 3*i;
	return vec3{t,t,t};
}

void Particles::_load(float **vbufs,float **sbufs)
{
		//fill buffers with random points
		for (int i = 0; i < this->vPrimitives(); i++) {
			vec3 point = point_ball(2) + vec3{-20.0f*(i%3)*(i%2),20.0f*(i%2)};
			vec4 vel = vec3{0,0,0};

			for (int k = 0; k < 4; k++) {
				int ind = 4*i + k;
				vbufs[POS][ind] = *point[k];
				vbufs[COL][ind] = ((i+1)%(k+1))*0.25;
				sbufs[VEL][ind] = *vel[k];
			}
			vbufs[COL][3] = 0;
		}

		for (int i = 0; i < std430BufSize(0); i++) {
			sbufs[ACC][i] = 0.0f;
			sbufs[COLLISION][i] = 0.0f;
		}
		
	return;
}


#endif