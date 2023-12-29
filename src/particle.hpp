#ifndef PARTICLE_HPP
#define PARTICLE_HPP
#include "particle.h"
#include "shapes.h"

void Particles::_init() {
	//define primitive sizes for each vertex buffer and create empty array for each one
	_vbo_primitives[PART_POS] = 4;
	_vbo_primitives[PART_COLOR] = 4;

	_vbo_layout[PART_POS] = 0;
	_vbo_layout[PART_COLOR] = 1;
	
	_ssbo_primitives[PART_VEL] = 4;
	_ssbo_layout[PART_VEL] = 0;
}

Particles::Particles(int count) : GLBufferWrapper<PART_VBOS,PART_SSBOS>(){
	_init();
	this->_nparts = count;
	this->_array_size = _nparts*NSTEPS;
	this->_ssbo_sizes[PART_VEL] = _nparts;

	_stepcounts = new int[2*_nparts];
	_firsts = new int[2*_nparts];

	this->initBuffers(GL_STREAM_DRAW);
}

/**
 * @brief Run specified compute shader on particle data.
 * 
 * SSBO bindings are arranged as follows:
 * 
 * binding 0: positions
 * binding 1: velocities
 * binding 2: colors
 * 
 * @param shader ComputeShader instance
 * @param groups Work groups
 */
void Particles::update(ComputeShader shader,matrix<1,3,int> groups)
{
	// Reset offset
	

	//set up bindings on GPU
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,_vbos[PART_POS]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,_ssbo[PART_VEL]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,2,_vbos[PART_COLOR]);

	shader.use();
	shader.setUniform("NPARTS", (unsigned int)_nparts);
	shader.setUniform("STEPS",(unsigned int)(NSTEPS-1));
	shader.setUniform("offset",(unsigned int)offset);
	shader.setUniform("t",(float)glfwGetTime());

	glDispatchCompute(
		(_nparts-1) / (*groups[0]) + 1,
		1,//(nparts+*groups[1]-1) / (*groups[1]), 
		1//(_count+*groups[2]-1) / (*groups[2])
	);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

	this->offset++;

	if (this->offset >= NSTEPS-1) 
		this->offset = 0;

	for (int i = 0; i < _nparts; i++) {
		_stepcounts[2*i] = offset;
		_stepcounts[2*i + 1] = NSTEPS - offset - 1;
		
		_firsts[2*i] = i*NSTEPS;
		_firsts[2*i+1] = i*NSTEPS + offset + 1;
	}	

	return;
}

void Particles::draw(ShaderProgram shader) {
	shader.use();
	shader.setUniform("geom_model", _model, GL_TRUE);
	shader.setUniform("t",(float)glfwGetTime());

	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	glBindVertexArray(_vao);
	glMultiDrawArrays(GL_LINE_STRIP, _firsts,_stepcounts,2*_nparts);

	glDepthMask(GL_TRUE);
	shader.setUniform("geom_model", mat4::id(), GL_FALSE);
}

vec3 random_in_bounds(vec3 bound) {
	float x = *bound[0]*uniform_rand();
	float y = *bound[1]*uniform_rand();
	float z = *bound[2]*uniform_rand();
	return vec3{x,y,z};
}


//these are just random configurations for generating points
inline vec3 point_ball(float r) {
	return Sphere(r*gaussian(1,0))(gaussian(PI/2,PI/5),gaussian(PI,PI));
}

inline vec3 i_torus(float rhole,float rtube, int i,int n) {
	int M = sqrt(n);
	float s = (2*PI*i)/n;
	float t = (PI*(i%M))/M;
	return Torus(rhole,rtube)(s,t);
}

inline vec3 path(int i,int n) {
	float t = 3*i;
	return vec3{t,t,t};
}

void Particles::_load(float **vbufs,float **sbufs)
{	

	//fill buffers with points
	// array contains nparts*NSTEPS 
	int M = sqrt(_nparts);
	for (int i = 0; i < this->_nparts; i++) {

		vec4 point = Sphere(15.0f)(2*i*PI/_nparts,PI/2) - vec3{0,0,0};

		vec4 vel = 0*cross(vec3{-3,1,-2},vec3(point-vec3{15,0,0}));
		vec4 color = vec3{1,1,1};

		for (int k = 0; k < 4*NSTEPS; k++) {
			int ind = 4*NSTEPS*i + k;
			vbufs[PART_POS][ind] = *point[k%4];
			vbufs[PART_COLOR][ind] = *color[k%4];
		}
		for (int k = 0; k < 4; k++) {
			sbufs[PART_VEL][4*i + k] = *vel[k];
		}
	}
	return;
}


#endif