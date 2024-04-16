#version 430 core

uniform float value;

in vec4 frag_color;
in vec3 frag_pos;
in vec3 frag_normal;
out vec4 FragColor;

float PI = 3.141592654;

void main()
{   
    FragColor = vec4(frag_color.xy,value,1);
} 