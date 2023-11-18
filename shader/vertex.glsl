#version 400 core

uniform float near_dist;
uniform float far_dist;
uniform mat4 cam_modelview;
uniform mat4 cam_projection;
uniform mat4 geom_model;

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec3 v_color;

out vec3 frag_color;
out vec3 frag_pos;

void main() {
	gl_PointSize = 20.0;
	
	vec4 v_pos_new = geom_model*vec4(v_pos,1);
	
	//lighting is based on geometry transformations
	frag_color = vec3(v_color); 
	frag_pos = vec3(v_pos_new);
	
	//now we do camera transformations
	v_pos_new = cam_projection*cam_modelview * v_pos_new;

	float w = v_pos_new.z/near_dist;
	float depth = 2*(v_pos_new.z - near_dist)/(far_dist - near_dist) - 1;

    gl_Position = vec4((v_pos_new).xy,depth,w);
}