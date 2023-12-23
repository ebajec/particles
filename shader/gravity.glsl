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

float dt = delta;
float r_collide = 1;
float G = 0.09;

void main() {
    uint idx = gl_GlobalInvocationID.x;
    uint idy = gl_GlobalInvocationID.y;
    
    vec3 pos = positions[idx];
    vec3 vel = velocities[idx];

    velocities[idx] += dt*accelerations[idx];
    accelerations[idx] = vec3(0,0,0);

    //calculate new values
    if (idy < nPoints && idy != idx)  {
        vec3 r = positions[idx] - positions[idy];
        float dist = -1;
        float tc = t_collide(positions[idx],positions[idy],velocities[idx],velocities[idy],r_collide);


        if (tc > 0 && tc < dt ){

            positions[idx] += velocities[idx]*tc;
            positions[idy] += velocities[idy]*tc;

            r = positions[idx] - positions[idy];
            dist = sqrt(dot(r,r));

            vec3 u = r/dist;

            vec3 uproj1x = u*dot(velocities[idx],u);
            vec3 uproj2x = u*dot(velocities[idy],u);

            vec3 uproj1y = velocities[idx] - uproj1x;
            vec3 uproj2y = velocities[idy] - uproj2x;

            velocities[idx] = 0.8*(uproj1y + uproj2x);
            velocities[idy] = 0.8*(uproj2y + uproj1x);
        }

        if (dist < 0) dist = sqrt(dot(r,r));

        //worst case check if balls end up colliding
        if (dist < 2*r_collide) {
            float diff = dist - 2*r_collide;
            positions[idx] -= 0.98*(diff/dist) * r;
            //positions[idy] += 0.5*(diff/dist) * r;
        }

        accelerations[idx] -= (G/(dist*dist))*r; 
    }

    barrier();
    positions[idx] += dt*velocities[idx];

    if (idx >= nPoints) {
        return;
    }
}





