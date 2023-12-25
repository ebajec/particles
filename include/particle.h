#ifndef PARTICLE_H
#define PARTICLE_H

#include <GLFW/glfw3.h> 
#include "bufwrapper.h"
#include "misc.h"

enum PART_VBO_ATTR {PART_POS,PART_COLOR};
enum PART_SSBO_ATTR {PART_VEL};

#define PART_VBOS 2
#define PART_SSBOS 1
#define NSTEPS 201

class Particles : public GLBufferWrapper<PART_VBOS,PART_SSBOS> {
public:
    Particles(int count);
    
    void update(ComputeShader shader,matrix<1,3,int> groups);
    void draw(ShaderProgram shader);

    double t_o = 0;
private:
    void _init();

    void _load(float** vbufs,float** sbufs);

    // Current timestep in loop
    int offset = 0;

    //number of points
    size_t nparts;
    
    //Used when calling glMultiDrawArrays to specify sizes of trails.
    int* _stepcounts;
    int* _firsts;
    
};

#include "particle.hpp"

#endif