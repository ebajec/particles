#version 400 core

in vec3 frag_color;
in vec3 frag_pos;
out vec4 FragColor;

float PI = 3.141592654;

void main()
{ 
    FragColor = vec4(frag_color,1);
} 