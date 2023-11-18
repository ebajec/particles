#version 430 core

layout(points) in; // Specify input type as points
layout(points, max_vertices = 1) out; // Specify output type as points, with a maximum of one vertex

void main() {
    // Pass through the point position from the vertex shader
    gl_Position = gl_in[0].gl_Position;

    // Emit the vertex to pass it to the next stage (fragment shader)
    EmitVertex();

    // End the primitive
    EndPrimitive();
}