#version 430 core

uniform uint NPARTS;
uniform uint STEPS;
uniform uint offset;
uniform float t;

layout(local_size_x = 256,local_size_y = 1,local_size_z = 1) in;

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

float dt = 0.005;
float r_collide = 0.5;
float G = 0.1;

void update_trail(uint ind, uint cur, uint next, vec3 pos_new) {
    positions[ind + next] = pos_new;

    float c = sigmoid((sqrt(dot(velocities[gl_GlobalInvocationID.x],velocities[gl_GlobalInvocationID.x]))/10-1));
    colors[ind+cur] = vec4(1-c/4,1-c,c,t);

    // Store current position at end of trail buffer if the offset has
    // reset to zero. This allows trails to be drawn continuously.
    if (next == 0) {
        positions[ind + STEPS] = pos_new;
        colors[ind+ STEPS] = vec4(1-c/4,1-c,c,t);
    }
}

vec3 forces(vec3 pos) {
    vec3 force = vec3(0,0,0);
    for (uint i = 0; i < NPARTS; i++) {
        if (i != gl_GlobalInvocationID.x) {
            vec3 r = pos -  positions[i*(STEPS + 1) + offset%STEPS];
            float dist = sqrt(dot(r,r));
            force -= (G/(pow(dist,2)+0.1)) * (r/dist);
        }    
    }
    return force;
}

vec3 RK4(vec3 x_0, float dt) {
    vec3 k_1 = forces(x_0);
    vec3 k_2 = forces(x_0 + dt*k_1/2);
    vec3 k_3 = forces(x_0 + dt*k_2/2);
    vec3 k_4 = forces(x_0 + dt*k_3);

    return x_0 + dt*(k_1 + 2*k_2 + 2*k_3 + k_4)/6;
}

void main() {
    uint idx = gl_GlobalInvocationID.x;
    uint idy = gl_GlobalInvocationID.y;

    uint pos_idx = idx*(STEPS+1);
    uint pos_idy = idy*(STEPS+1);

    uint cur = offset%STEPS;
    uint next = (offset+1)%STEPS;

    velocities[idx] += RK4(positions[pos_idx + cur],dt) - positions[pos_idx + cur];
    
    barrier();

    update_trail(pos_idx,cur,next,positions[pos_idx + cur] + dt*velocities[idx]);

    if (next == 0) {
        positions[pos_idx + STEPS] = positions[pos_idx + next];
    }

    if (idx > NPARTS) {
        return;
    }
}



//vec3 v1 = velocities[idx];
        //vec3 v2 = velocities[idy];
        //float dist = -1;
        //float tc = -1;//t_collide(p1,p2,v1,v2,r_collide);
        //if (tc > 0 && tc < dt ){
        //    positions[pos_idx + cur] = p1 + v1*tc;
        //    positions[pos_idy + cur] = p2 + v2*tc;
//
        //    r = positions[pos_idx+cur] - positions[pos_idy+cur];
        //    dist = sqrt(dot(r,r));
//
        //    vec3 u = r/dist;
//
        //    vec3 uproj1x = u*dot(v1,u);
        //    vec3 uproj2x = u*dot(v2,u);
//
        //    vec3 uproj1y = v1 - uproj1x;
        //    vec3 uproj2y = v2 - uproj2x;
//
        //    velocities[idx] = (uproj1y + uproj2x);
        //    velocities[idy] = (uproj2y + uproj1x);
        //}


