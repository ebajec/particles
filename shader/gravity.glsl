#version 430 core

uniform int nPoints;
uniform float delta;
uniform float t;

layout(local_size_x = 64,local_size_y = 16,local_size_z = 1) in;

layout(std430, binding = 0) buffer Positions{
    vec3 positions[];
};

layout(std430, binding = 1) buffer Velocities{
    vec3 velocities[];
};

layout(std430, binding = 2) buffer Accelerations{
    vec3 accelerations[];
};

layout(std430, binding = 3) buffer Color{
    vec4 colors[];
};

layout(std430, binding = 4) buffer Collision{
    float collisions[];
};

float sigmoid(float x ){
    return 1/(1+exp(-x));
}

vec3 attractor(vec3 pos) {
    vec3 r = vec3(50,0,0) - pos;
    return (0.0000001/pow(dot(r,r),3/2))*r;
}


float G = 0.0001;


void main() {

    float dt = delta;
    float r_collide = 0.5;

    uint idx = gl_GlobalInvocationID.x;
    uint idy = gl_GlobalInvocationID.y;
    
    vec3 pos = positions[idx];
    vec3 vel = velocities[idx];

    //calculate new values
    if (idy < nPoints) { 

        velocities[idx] += dt*accelerations[idx];
        accelerations[idx] = vec3(0,0,0);

        if (idy != idx)  {
            vec3 r = positions[idy] - pos;

            float dist = sqrt(dot(r,r));
            float diff = 0.5*dist - r_collide;

            if (colors[idy].w < 1 && diff < 0) {
                vec3 u = -r/dist;
                velocities[idx] -= dot(velocities[idx],u);
                velocities[idy] -= dot(velocities[idy],u);

                positions[idx] += 1.1*diff*u;
                positions[idy] -= 1.1*diff*u;

                colors[idy].w = 2;
                colors[idx].w = 2;
            }
            accelerations[idx] += (G/pow(dist,2))*r;    
        }
    
        barrier();
        positions[idx] += dt*vel;
        
    }
    barrier();

    if (idx >= nPoints) {
        return;
    }
}





