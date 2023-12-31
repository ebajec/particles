#version 430 core

uniform float near_dist;
uniform float far_dist;
uniform mat4 cam_modelview;
uniform mat4 cam_projection;
uniform mat4 geom_model;

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec4 v_color;

out vec4 frag_color;
out vec3 frag_pos;
//out vec3 frag_normal;

void main() {
	
	vec4 v_pos_new = geom_model*vec4(v_pos,1);
	//Make it so z is up
	v_pos_new = v_pos_new.xzyw;
	//vec4 normal_new = geom_model*vec4(normal,1);
	
	//lighting is based on geometry transformations
	frag_color = vec4(v_color); 
	frag_pos = vec3(v_pos_new);
	//frag_normal = vec3(normal_new);
	
	//now we do camera transformations
	v_pos_new = cam_projection*cam_modelview * v_pos_new;

	float w = v_pos_new.z/near_dist;
	float depth = 2*(v_pos_new.z - near_dist)/(far_dist - near_dist) - 1;

    gl_Position = vec4((v_pos_new).xy,depth,w);
}