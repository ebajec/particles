#version 430 core

uniform uint NPARTS;
uniform uint STEPS;
uniform uint offset;
uniform float t;

layout(local_size_x = 64,local_size_y = 16,local_size_z = 1) in;

layout(std430, binding = 0) buffer Positions{
    vec3 positions[];
};

layout(std430, binding = 1) buffer Velocities{
    vec3 velocities[];
};

layout(std430, binding = 2) buffer Color{
    vec4 colors[];
};


float sigmoid(float x ){
    return 1/(1+exp(-x));
}

// Considering the paths x1 + t*v1 and x2 + t*v2, we check the first value of t for which
// the particles would collide on this path. 
float t_collide(vec3 x1,vec3 x2, vec3 v1, vec3 v2, float rc) {
    vec3 xdiff = x1-x2;
    vec3 vdiff = v1-v2;
    float dotxv = dot(xdiff,vdiff);

    if (dot(v1,v1) + dot(v2,v2) < 1e-6) return 0;

    float D = 4*dotxv*dotxv-4*dot(vdiff,vdiff)*(dot(xdiff,xdiff) - 4*rc*rc);

    if (D < 0) {
        return -1;
    }
    else {
        return (-dotxv-0.5*sqrt(D))/dot(vdiff,vdiff);
    } 
}

float dt = 0.01;
float r_collide = 0.5;
float G = 0.1;

void main() {
    uint idx = gl_GlobalInvocationID.x;
    uint idy = gl_GlobalInvocationID.y;

    uint pos_idx = idx*(STEPS+1);
    uint pos_idy = idy*(STEPS+1);

    uint cur = offset%STEPS;
    uint next = (offset+1)%STEPS;

    //calculate new values
    if (idy < NPARTS && idy != idx)  {
        vec3 p1 = positions[pos_idx + cur];
        vec3 p2 = positions[pos_idy + cur];
        
        vec3 r = p1 - p2;

        vec3 v1 = velocities[idx];
        vec3 v2 = velocities[idy];
        float dist = -1;
        float tc = -1;//t_collide(p1,p2,v1,v2,r_collide);

        if (tc > 0 && tc < dt ){
            positions[pos_idx + cur] = p1 + v1*tc;
            positions[pos_idy + cur] = p2 + v2*tc;

            r = positions[pos_idx+cur] - positions[pos_idy+cur];
            dist = sqrt(dot(r,r));

            vec3 u = r/dist;

            vec3 uproj1x = u*dot(v1,u);
            vec3 uproj2x = u*dot(v2,u);

            vec3 uproj1y = v1 - uproj1x;
            vec3 uproj2y = v2 - uproj2x;

            velocities[idx] = (uproj1y + uproj2x);
            velocities[idy] = (uproj2y + uproj1x);
        }
        vec3 acc = (-G/dot(r,r))*r; 
        velocities[idx] += dt*acc;
    }
    barrier();

    colors[pos_idx+cur].w = t;
    float c = sigmoid((sqrt(dot(velocities[idx],velocities[idx]))/3-3));
    colors[pos_idx+cur].xyz = vec3(1-c,1-c/2,1);

    positions[pos_idx + next] = positions[pos_idx + cur] + dt*velocities[idx];

    if (next == 0) {
        positions[pos_idx + STEPS] = positions[pos_idx + next];
    }

    if (idx >= NPARTS) {
        return;
    }
}





