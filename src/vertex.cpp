//#ifndef VERTEX_HPP
//#define VERTEX_HPP
#include "vertex.h"


Vertex::Vertex(const vec3& v, const vec3& normal, const vec3& color)
{
	this->color = color;
	this->position = v;
	this->normal = normal;
	this->degree = 0;
}

Vertex::~Vertex() {
	for (Vertex* v : connections) {
		if (v != nullptr) v->connections.erase(this);
	}

	for (Face* F : faces) {
		for (Vertex* v : F->vertexArray()) {
			if (v != this) v->faces.erase(F);
		}
	}
}

void Vertex::connect(Vertex* other)
{
	//check to avoid incrementing degree unnecessarily
	if (this == other || this->connections.find(other) != this->connections.end()) {
		return;
	}
	else {
		this->connections.insert(other);
		other->connections.insert(this);

		Edge* E = new Edge(this, other);

		this->degree++;
		other->degree++;
	}
	return;
}

void Vertex::disconnect(Vertex* other)
{
	//check to avoid decrementing degree unnecessarily
	if (this->connections.find(other) == this->connections.end()) {
		return;
	}
	else {
		Edge* E = this->edges.at(other);
		other->edges.erase(this);
		this->edges.erase(other);
		this->connections.erase(other);
		other->connections.erase(this);
		this->degree--;

		delete(E);
	}
	return;
}

bool Vertex::comparator::operator () (const Vertex* v1, const Vertex* v2) const {
	vec3 pos1 = v1->position;
	vec3 pos2 = v2->position;
	for (int i = 0; i < 3; i++) {
		if (abs(pos1[0][i] - pos2[0][i]) > 0.01) {
			return false;
		}
	}
	return true;
}

size_t Vertex::hasher::operator ()(const Vertex* v) const {
	vec3 pos = v->position;
	size_t seed = 3;
	for (int i = 0; i < 3; i++) {
		seed ^= std::hash<GLfloat>()(pos[0][i]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
	return seed;
}

//EDGE
void Edge::init() {
	this->_vertices = vector<Vertex*>(2);
}

Edge::Edge(Vertex* first, Vertex* second) {
	init();
	this->_vertices[0] = first;
	this->_vertices[1] = second;
	first->edges.insert({second,this});
	second->edges.insert({first,this});
}

//Returns adjacent face to F on an edge, and nullptr if there is none.
Face* Edge::adjacent(Face* F) {
	Face* adj = nullptr;

	for (Face* F_other : faces) {
		if (F_other != F) return F_other;
	}
}

size_t VertexRelation::Hasher::operator () (const VertexRelation* E) const {

	size_t pointer_sum = 0;
	//modulo UNINT64_MAX/size is to avoid overflow
	for (Vertex* v : E->vertexArray()) pointer_sum += (size_t)v % (UINT64_MAX / E->size());

	return pointer_sum;
}


//FACE
Face::Face(std::initializer_list<Vertex*> args) {
	this->_vertices = vector<Vertex*>(args.size());
	this->edges = vector<Edge*>(args.size());
	
	{int i = 0; for (Vertex* v : args) {
		_vertices[i] = v;
		v->faces.insert(this);
		i++;
	}}


	for (int i = 0; i < this->size(); i++) {
		Vertex* v = _vertices[i];
		for (Vertex* w : v->connections) {
			if (w == _vertices[modulo(i+1,this->size())]) {
				Edge* E = v->edges.at(w);
				edges[i] = E;
				E->faces.push_back(this);
				break;
			}
		}
	}
}
	
void Face::computeNormal() {
	vec3 v1 = _vertices[1]->position - _vertices[0]->position;
	vec3 v2 = _vertices[2]->position - _vertices[0]->position;
	normal = cross(v1, v2);
 }


bool Face::Comparator::operator () (const VertexRelation* F1, const VertexRelation* F2)  const {
	if (F1->size() != F2->size()) return false;

	int size = F1->size();
	Vertex** data_F2 = F2->data();

	for (int i = 0; i < size; i++) {
		if (!linear_search<Vertex*>(data_F2, size, F1->get(i))) {
			return false;
		}
	}

	return true;
}


//#endif

