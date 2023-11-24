#ifndef PARTICLE_H
#define PARTICLE_H

#include <GLFW/glfw3.h> 
#include "drawable.h"
#include "misc.h"

enum PART_VBO_ATTR {POS,COL};
enum PART_SSBO_ATTR {VEL,ACC,COLLISION};
enum DIST_TYPE {RAND, FUNC};

#define PART_VBOS 2
#define PART_SSBOS 3

class Particles : public GLBufferWrapper<PART_VBOS,PART_SSBOS> {
public:
    Particles(int count,vec3 bound = vec3{1,1,1});
    
    void update(ComputeShader shader);
    void draw_trails(ShaderProgram shader);

    double t_o = 0;
private:
    void _init();

    unsigned long vPrimitives(){return _count;}

    
    void _load(float** vbufs,float** sbufs);

    //top right corner of bounding box
    vec3 _bound;

    //number of points
    size_t _count;
    
    //previous time instance.
    
};

#include "particle.hpp"

#endif