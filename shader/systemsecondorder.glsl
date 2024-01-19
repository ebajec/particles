#version 430 core

uniform uint NPARTS;
uniform uint STEPS;
uniform uint offset;
uniform float t;

float PI = 3.141592654;

// Big because usually lots of particles
layout(local_size_x = 128,local_size_y = 1,local_size_z = 1) in;

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

vec3 sigmoid3(vec3 x ){
    return vec3(sigmoid(x.x),sigmoid(x.y),sigmoid(x.z));
}


float a = 40; //damping term
float b = 0.1;
float c = 0.4;
float d = 0.7;

//cool looking thing
vec3 g(vec3 pos) {
    return mat3(
        -a,          b*pos.y,     -c*pos.z,
        -b*pos.y,     -a ,         d*pos.x,
       c*pos.z,     -d/3*pos.x,    -a  
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

float timestep = 0.01;


vec3 hsvtorgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void update_trail(uint ind, uint cur, uint next, vec3 pos_new) {
    positions[ind + next] = pos_new;

    // Hue is proportional to speed, alpha value is inversely proportional
    float M = sqrt(dot(velocities[gl_GlobalInvocationID.x],velocities[gl_GlobalInvocationID.x]));
    colors[ind+cur] = vec4(hsvtorgb(vec3(sigmoid(M/300),1,1)),1/(M+1));

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







