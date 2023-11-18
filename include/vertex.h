#pragma once
#ifndef VERTEX_H
#define VERTEX_H
#include "matrix.h"
#include <queue>
#include "misc.h"
#include <unordered_set>
#include <unordered_map>
#include <GL/glew.h>
#include <vector>
#include <forward_list>


#define VERTEX_ATTRIBUTES 4

using namespace std;

enum VERTEX_ATTRIBUTE {
	POSITION,
	NORMAL,
	COLOR,
	ADJACENCY
};

//Vertex
using vec3 = matrix<3, 1, GLfloat>;

class Vertex;
class VertexRelation;
class Edge;
class Face;

class Vertex {
public:

	struct comparator {
		bool operator () (const Vertex* v1, const Vertex* v2) const;
	};

	struct hasher {
		size_t operator ()(const Vertex* v) const;
	};

	vec3 position;
	vec3 normal;
	vec3 color = { 1.0f,1.0f,1.0f };
	unordered_set<Vertex*> connections;
	unordered_set<Face*> faces;
	unordered_map<Vertex*,Edge*> edges;

	Vertex() { degree = 0; }
	Vertex(const vec3& v, const vec3& normal, const vec3& color = { 1.0f,1.0f,1.0f });
	~Vertex();

	void connect(Vertex* other);
	void disconnect(Vertex* other);
	template<typename... Args>
	void connect(Vertex* arg, Args... args);
	template<typename... Args>
	void disconnect(Vertex* arg, Args... args);

	size_t deg() { return degree; }


private:
	size_t degree;
};

inline void common_edge(Vertex* v1, Vertex* v2) {

}


using mat3 = matrix<3, 3, GLfloat>;


//this is used to represent face and edge relationships between vertices
//
class VertexRelation {

public:
	vector<Vertex*> vertexArray() const { return _vertices; }
	Vertex* get(int index) const { return _vertices[index]; }
	int size() const { return _vertices.size(); }
	Vertex** data() const { return (Vertex**)_vertices.data(); }

	struct Hasher {
		size_t operator () (const VertexRelation* E) const;
	};

protected:
	vector<Vertex*> _vertices;
};

//Edge representing connecting between two vertices. These are meant to be unique
//objects in memory for each connection between a pair of vertices.
class Edge : public VertexRelation {
public:
	Edge() { init(); }
	Edge(Vertex* first, Vertex* second);

	Face* adjacent(Face* F);

	vector<Face*> faces;
private:
	void init();
};

//Represents a face formed by a set of vertices.
class Face : public VertexRelation {
public:
	Face() {}

	Face(std::initializer_list<Vertex*> args);

	struct Comparator {
		bool operator () (const VertexRelation* F1, const VertexRelation* F2) const;
	};

	void computeNormal();

	vector<Edge*> edges;
	vec3 normal = { 0,0,0 };
};

inline forward_list<Vertex*> common_vertices(Vertex* v, Vertex* w) {
	forward_list<Vertex*> common;
	for (Vertex* cv : v->connections) {
		for (Vertex* cw : w->connections) {
			if (cv == cw) {
				common.push_front(cv);
			}
		}
	}
	return common;
}

template<typename func>
inline void bfs(Vertex* start, func s) {
	std::queue<Vertex*> queue;
	unordered_set<Vertex*> visited;

	queue.push(start);
	visited.insert(start);

	while (!queue.empty()) {
		Vertex* dequeued = (Vertex*)queue.front();
		queue.pop();
		s(dequeued);

		//Mark connections as visited and add unvisited to queue
		for (Vertex* v : dequeued->connections) {

			//Only queue the vertex if insert() returns true for new element insertion
			bool new_insertion = visited.insert(v).second;

			if (new_insertion) {
				queue.push(v);
			}
		}

	}
}

template<typename ...Args>
void Vertex::connect(Vertex* arg, Args ...args)
{
	connect(arg);
	connect(args...);
}

template<typename ...Args>
void Vertex::disconnect(Vertex* arg, Args ...args)
{
	disconnect(arg);
	disconnect(args...);
}

#endif


