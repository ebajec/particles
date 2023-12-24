#version 430 core

uniform float t;

in vec4 frag_color;
in vec3 frag_pos;
in vec3 frag_normal;
out vec4 FragColor;

float PI = 3.141592654;

void main()
{   
    float fade = exp((frag_color.w - t));
    FragColor = vec4(frag_color.xyz,fade);
} 