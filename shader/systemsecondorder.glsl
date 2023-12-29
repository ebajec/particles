#version 430 core

uniform uint NPARTS;
uniform uint STEPS;
uniform uint offset;
uniform float t;

// Big because usually lots of particles
layout(local_size_x = 256,local_size_y = 1,local_size_z = 1) in;

layout(std430, binding = 0) buffer Positions{
    vec3 positions[];
};

layout(std430, binding = 1) buffer Velocities{
    vec3 velocities[];
};

layout(std430, binding = 2) buffer Colors{
    vec4 colors[];
};

float sigmoid(float x ){
    return 1/(1+exp(-x));
}

// Lorenz attractor
float sigma = 10;
float rho = 28;
float beta = 2;
vec3 lorenz(vec3 pos) {
    return vec3(
        sigma*(pos.y-pos.x),
        pos.x*(rho - pos.z) - pos.y,
        pos.x*pos.y - beta*pos.z
    );
}


float a = 100; //damping term
float b = 2;
float c = 2;
float d = 4;

//cool looking thing
vec3 g(vec3 pos) {
    return mat3(
        -a,          b*pos.y,     -c*pos.z,
        -b*pos.y,     -a ,         d*pos.x,
       c*pos.z,     -d*pos.x,    -a  
    )*pos;
}

vec3 system(vec3 pos) {
    return g(pos);
}


vec3 RK4(vec3 x_0, float dt) {
    vec3 k_1 = system(x_0);
    vec3 k_2 = system(x_0 + dt*k_1/2);
    vec3 k_3 = system(x_0 + dt*k_2/2);
    vec3 k_4 = system(x_0 + dt*k_3);

    return x_0 + dt*(k_1 + 2*k_2 + 2*k_3 + k_4)/6;
}

float timestep = 0.004;


void update_trail(uint ind, uint cur, uint next, vec3 pos_new) {
    positions[ind + next] = pos_new;

    float c = sigmoid((sqrt(dot(velocities[gl_GlobalInvocationID.x],velocities[gl_GlobalInvocationID.x]))/20-2.6));
    colors[ind+cur] = vec4(1-c/3,1-c/4,c,t);

    // Store current position at end of trail buffer if the offset has
    // reset to zero. This allows trails to be drawn continuously.
    if (next == 0) {
        positions[ind + STEPS] = pos_new;
        colors[ind+ STEPS] = colors[ind + cur];
    }
}

void main() {
    uint idx = gl_GlobalInvocationID.x;

    if (idx > NPARTS) {
        return;
    }

    uint ind = idx*(STEPS+1);
    uint cur = offset%STEPS;
    uint next = (offset+1)%STEPS;

    velocities[idx] += RK4(positions[ind + cur],timestep) - positions[ind+ cur];

    update_trail(ind,cur,next,positions[ind + cur] + timestep*velocities[idx]);

}







