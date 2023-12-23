#pragma once
#ifndef MESH_H
#define MESH_H

#include <initializer_list>
#include <vector>
#include <map>
#include <GL/glew.h>
#include "bufwrapper.h"
#include "vertex.h"
#include "matrix.h"
#include "shapes.h"

void center(Vertex* start);
vec3 centroid(Vertex* start);

enum ShapeType { LINE = 2, TRIANGLE = 3 };

class Mesh : public GLBufferWrapper<VERTEX_ATTRIBUTES,0> {

public:
	Mesh();
	template<int n>
	Mesh(matrix<n, n, int> adjacency, initializer_list<vec3> vertices);
	Mesh(Vertex* vertex, bool center_vertices = true);
	template<typename paramFunc>
	Mesh(Surface<paramFunc> S, int genus, int N_s, int N_t);

	~Mesh();

	void center();
	void colorCurvature(float angle=PI);
	void computeNormals();
	void setType(ShapeType type);

	//checks V - E + F to assess whether meshes are being processed properly
	void checkChar();

	// Retrieves memory from specified attribute buffer on GPU, then passes pointer
	// and buffer size to F.  F must be a callable object taking a pointer to float 
	// and an int.   
	template<typename func> void transformCPU(VERTEX_ATTRIBUTE attribute, func F);
	template<typename func> void transformPositionsCPU(func F);

	//Directly calls F on each vertex in the mesh.  F must be a callable object 
	//taking a Vertex pointer.
	template<typename func> void transformVertices(func F);

	unsigned long arraySize();

	const list<Face*> faces() {return _face_list;}
	const list<Edge*> edges() {return _edge_list;}

protected:
	vector<Vertex*> _vertex_list;
	list<Face*> _face_list;
	list<Edge*> _edge_list;

	map<Face*, int> _face_indices;
	GLuint face_normal_buffer;
	GLuint position_storage_buffer;

	ShapeType _type = TRIANGLE;

	void _init();
	
	void _findFacesTriangular();
	void _findEdges();
	void _load(float** vbufs,float** sbufs);
};

#include "mesh.hpp"

#endif
