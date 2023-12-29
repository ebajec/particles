#version 430 core

uniform uint NPARTS;
uniform uint STEPS;
uniform uint offset;
uniform float t;

layout(local_size_x = 32,local_size_y = 1,local_size_z = 1) in;

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

float a = 0.2;
float b = 0.1;
float c = 5.7;

// Rossler attractor
vec3 f(vec3 pos) {
    return vec3(
        -pos.y - pos.z,
        pos.x + a*pos.y,
        b + pos.z*(pos.x-c)
    );
}

// Multiscroll attractor
vec3 g(vec3 pos) {
    return vec3(
        a*(pos.y - pos.x),
        pos.x - pos.x*pos.z + c*pos.y - 15.15,
        pos.x*pos.y  - b*pos.z
    );
}


vec3 RK4(vec3 x_0, float dt) {
    vec3 k_1 = lorenz(x_0);
    vec3 k_2 = lorenz(x_0 + dt*k_1/2);
    vec3 k_3 = lorenz(x_0 + dt*k_2/2);
    vec3 k_4 = lorenz(x_0 + dt*k_3);

    return x_0 + dt*(k_1 + 2*k_2 + 2*k_3 + k_4)/6;
}

float timestep = 0.007;

vec3 hsvtorgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void update_trail(uint ind, uint cur, uint next, vec3 pos_new) {
    positions[ind + next] = pos_new;

    // Hue is proportional to speed, alpha value is inversely proportional
    float M = sqrt(dot(velocities[gl_GlobalInvocationID.x],velocities[gl_GlobalInvocationID.x]));
    colors[ind+cur] = vec4(hsvtorgb(vec3(sigmoid(M/220),1,1)),1/(M+1));

    // Store current position at end of trail buffer if the offset has
    // reset to zero. This allows trails to be drawn continuously.
    if (next == 0) {
        positions[ind + STEPS] = pos_new;
        colors[ind+ STEPS] = colors[ind + next];
    }
}

void main() {
    uint idx = gl_GlobalInvocationID.x;

    if (idx >= NPARTS) {
        return;
    }

    uint ind = idx*(STEPS+1);
    uint cur = offset%STEPS;
    uint next = (offset+1)%STEPS;

    vec3 pos_next = RK4(positions[ind+cur],timestep);

    velocities[idx] = (pos_next - positions[ind + cur])/timestep;

    update_trail(ind,cur,next,pos_next);

}







