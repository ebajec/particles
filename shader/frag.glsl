#version 430 core

uniform float t;

in vec4 frag_color;
in vec3 frag_pos;
in vec3 frag_normal;
out vec4 FragColor;

float PI = 3.141592654;

void main()
{   
    float fade = exp(5*(frag_color.w - t));
    //float fade = frag_color.w;
    FragColor = vec4(frag_color.xyz,fade);
} 