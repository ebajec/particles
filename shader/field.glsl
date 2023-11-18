#version 430 core

uniform int nPoints;
uniform float delta;
uniform float t;

layout(local_size_x = 256,local_size_y = 1,local_size_z = 1) in;

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
    vec3 colors[];
};

float sigmoid(float x ){
    return 1/(1+exp(-x));
}

vec3 circ(float s) {
    return vec3(cos(s-t/2),0,sin(s-t/2));
}

vec3 lorenz(vec3 pos, float sigma, float rho, float beta) {
    return vec3(sigma*(pos.y-pos.x),pos.x*(rho - pos.z) - pos.y,pos.x*pos.y - beta*pos.z);
}


void main() {
    uint idx = gl_GlobalInvocationID.x;

    if (idx >= nPoints) {
        return;
    }

    float dt = 0.00001*delta;

    vec3 pos = positions[idx];
    vec3 vel = velocities[idx];
    vec3 acc = accelerations[idx];

    mat3 A = mat3(
        -1,-1,0,
        1,0,-1,
        0,1,-1);

    positions[idx] += dt*vel;
    velocities[idx] += A*pos;
    accelerations[idx] = A*pos/dot(pos,pos);

    float c = sigmoid(dot(vel,vel)/10);
    colors[idx] =vec3(c,c,c);

}