#version 430 core

uniform uint NPARTS;
uniform uint STEPS;
uniform uint offset;
uniform float t;

//SYSTEM PARAMETERS
uniform float damp;
uniform float a;
uniform float b;
uniform float c;
uniform float f;

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

//cool looking thing
vec3 g(vec3 pos,vec3 vel) {
    float r = sqrt(pos.x*pos.x + pos.y*pos.y + pos.z*pos.z);
    float d = exp(-damp*r);

    return mat3(
        -f*d,          -a*pos.x,     -b*pos.y,
        a*pos.x,     -f*d,         -c*pos.z,
        b*pos.y,     c*pos.z,    -f*d
    )*vel ;
}

vec3 system(vec3 pos, vec3 vel) {
    return g(pos, vel);
}

vec3 hsvtorgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void update_trail(uint ind, uint cur, uint next, vec3 pos_new) {
    positions[ind + next] = pos_new;

    // Hue is proportional to speed, alpha value is inversely proportional
    float M = sqrt(dot(velocities[gl_GlobalInvocationID.x],velocities[gl_GlobalInvocationID.x]));
    colors[ind+cur] = vec4(hsvtorgb(vec3(sigmoid(M/100),1,1)),t);

    // Store current position at end of trail buffer if the offset has
    // reset to zero. This allows trails to be drawn continuously.
    if (next == 0) {
        positions[ind + STEPS] = pos_new;
        colors[ind+ STEPS] = colors[ind + cur];
    }
}

float dt = 0.01;

// Taylor approximations
vec3 T2(vec3 x, vec3 x_dot, vec3 x_ddot, float h) {
    return x + x_dot*h + x_ddot*pow(h,2)/2;
} 

vec3 T1(vec3 x, vec3 x_dot, float h) {
    return x + x_dot*h;
} 

void main() {
    uint idx = gl_GlobalInvocationID.x;

    if (idx > NPARTS) {
        return;
    }

    uint ind = idx*(STEPS+1);
    uint cur = offset%STEPS;
    uint next = (offset+1)%STEPS;

    // Fourth-order Runge-Kutta approximation 
    vec3 x_0 = positions[ind + cur];
    vec3 v_0 = velocities[idx];

    vec3 k_1 = system(x_0, v_0);
    vec3 v_1 = T1(v_0,k_1,dt/2);

    vec3 k_2 = system(T2(x_0,v_1,k_1,dt/2), v_1);
    vec3 v_2 = T1(v_1,k_2,dt/2);

    vec3 k_3 = system(T2(x_0,v_2,k_2,dt/2), v_2);
    vec3 v_3 = T1(v_1,k_3,dt);

    vec3 k_4 = system(T2(x_0,v_3,k_3,dt), v_3);

    vec3 v_final = v_0 + dt*(k_1 + 2*k_2 + 2*k_3 + k_4)/6;
    vec3 x_final = T2(x_0,v_0, v_final - v_0, dt);
    
    velocities[idx] = v_final;
    update_trail(ind,cur,next,x_final);
}







