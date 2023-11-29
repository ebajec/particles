#ifndef PARTICLE_HPP
#define PARTICLE_HPP
#include "particle.h"
#include "shapes.h"

void Particles::_init() {
	//define primitive sizes for each vertex buffer and create empty array for each one
	_vbo_primitives[POS] = 4;
	_vbo_primitives[COL] = 4;

	_vbo_layout[POS] = 0;
	_vbo_layout[COL] = 1;
	
	_ssbo_primitives[VEL] = 4;
	_ssbo_primitives[ACC] = 4;
	_ssbo_primitives[COLLISION] = 1;

	_ssbo_layout[VEL] = 0;
	_ssbo_layout[ACC] = 1;
	_ssbo_layout[ACC] = 2;
}

Particles::Particles(int count,vec3 bound) : GLBufferWrapper<PART_VBOS,PART_SSBOS>(){
	_init();
	this->_count = count;
	this->_bound = bound;
	this->_ssbo_sizes[ACC] = _count;
	this->_ssbo_sizes[VEL] = _count;
	this->_ssbo_sizes[COLLISION] = _count*_count;


	this->initBuffers(GL_STREAM_DRAW);
}

/**
 * @brief Run specified compute shader on particle data.
 * 
 * SSBO bindings are arranged as follows:
 * 
 * binding 0: positions
 * binding 1: velocities
 * binding 2: accelerations
 * binding 3: colors
 * binding 4: collision data (doesn't do anything right now, but I might need to 
 * keep track of them later)
 * 
 * @param shader ComputeShader instance
 * @param groups Work groups
 */
void Particles::update(ComputeShader shader,matrix<1,3,int> groups)
{
	//set up bindings on GPU
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,_vbos[POS]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,_ssbo[VEL]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,2,_ssbo[ACC]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,3,_vbos[COL]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,4,_vbos[COLLISION]);

	double t = glfwGetTime();

	shader.use();
	shader.setUniform("nPoints", (int)_count);
	shader.setUniform("delta",(float)(0.1) );
	shader.setUniform("t", (float)(t));

	glDispatchCompute(
		(_count+*groups[0]-1) / (*groups[0]),
		(_count+*groups[1]-1) / (*groups[1]), 
		1//(_count+*groups[2]-1) / (*groups[2])
	);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

	return;
}


vec3 random_in_bounds(vec3 bound) {
	float x = *bound[0]*uniform_rand();
	float y = *bound[1]*uniform_rand();
	float z = *bound[2]*uniform_rand();
	return vec3{x,y,z};
}


//these are just random configurations for generating points
inline vec3 point_ball(float r) {
	return Sphere(r*gaussian(0.9,0.05))(gaussian(PI/2,PI/5),gaussian(PI,PI));
}

inline vec3 i_torus(float rhole,float rtube, int i,int n) {
	float s = (2*PI*i)/n;
	float t = (PI*i)/(2.0f*n);
	return Torus(rhole,rtube)(s,t);
}

inline vec3 path(int i,int n) {
	float t = 3*i;
	return vec3{t,t,t};
}

void Particles::_load(float **vbufs,float **sbufs)
{	
	vector<vec3> points(_count);
	vector<vec3> vels(_count);

	//fill buffers with random points
	for (int i = 0; i < this->vPrimitives(); i++) {
		vec3 point = point_ball(15.0f);//i_torus(30.0f,5.0f,i,vPrimitives());
		vec4 vel = vec3{0,0,0};

		for (int k = 0; k < 4; k++) {
			int ind = 4*i + k;
			vbufs[POS][ind] = *point[k];
			vbufs[COL][ind] = 1*((i%4)==k);
			sbufs[VEL][ind] = *vel[k];
		}

		vbufs[COL][3] = 0;
	}
	for (int i = 0; i < std430BufSize(0); i++) {
		sbufs[ACC][i] = 0.0f;
	}
	for (int i = 0; i < ssboBufSize(COLLISION); i++) {
		sbufs[COLLISION][i] = 0.0f;
	}
		
	return;
}


#endif