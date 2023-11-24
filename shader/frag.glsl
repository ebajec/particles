#version 430 core

in vec3 frag_color;
in vec3 frag_pos;
in vec3 frag_normal;
out vec4 FragColor;

float PI = 3.141592654;

vec3 calc_light(vec3 light, vec3 color) {
    vec3 ray = light - frag_pos;
    float dist_sq = dot(ray,ray);
    float cosine = dot(frag_normal,ray)/sqrt(dist_sq);

    float brightness = 200*cosine/dist_sq;

    if (acos(cosine) > PI/2) {
       return vec3(0);
    }

    return brightness * color;
}

void main()
{ 
    vec3 light1 = vec3( -10,10,-10);
    vec3 light2 = vec3(10,-10,4);
    vec3 light3 = vec3(0,0,0);

    vec3 color = 0.2*frag_color + calc_light(light1,frag_color) + calc_light(light2,frag_color) + calc_light(light3,frag_color);

    FragColor = vec4(frag_color,1);
} 