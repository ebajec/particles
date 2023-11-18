#pragma once

#ifndef SHAPES_H
#define SHAPES_H

#include <stdlib.h>
#include "vertex.h"
#include <unordered_set>
#include <complex>
#include <vector>

#ifndef PI
#define PI 3.14159265359f
#endif
#define ROOT3OVER2 1.0f

constexpr GLfloat EPSILON = std::numeric_limits<GLfloat>::epsilon();

using namespace std;
using mat3 = matrix<3, 3, GLfloat>;
using vertex_set = unordered_set <Vertex*, Vertex::hasher, Vertex::comparator>;

struct Shape {};

struct Torus : public Shape {
	float r = 0.5f;
	float R = 1.0f;

	Torus(float r_hole, float r_tube) : R(r_hole), r(r_tube) {}

	vec3 operator () (float s, float t) {
		return vec3{ cos(s) * (R + r * cos(t)), sin(s) * (R + r * cos(t)), r * sin(t) };
	}
};

struct Sphere : public Shape {
	float r = 1.5f;

	Sphere(float radius) : r(radius) {}

	vec3 operator () (float s, float t) {
		return r * vec3{ sin(s) * cos(t), sin(s) * sin(t), cos(s) };
	}
};

struct BoySurface : public Shape {
	BoySurface() {}

	vec3 operator () (float s, float t) {
		std::complex<double> w = (double)s * exp((double)t * 1i);
		double rt5 = sqrt(5);

		std::complex<double> g_1 = -3.0f / 2.0f *
			imag(
				w * (1.0 - pow(w, 4)) /
				(pow(w, 6) + rt5 * pow(w, 3) - 1.0)
			);
		std::complex<double> g_2 = -3.0f / 2.0f * real(
			w * (1.0 + pow(w, 4)) /
			(pow(w, 6) + rt5 * pow(w, 3) - 1.0)
		);
		std::complex<double> g_3 = imag(
			(1.0 + pow(w, 6)) /
			(pow(w, 6) + rt5 * pow(w, 3) - 1.0)
		) - 0.5;

		matrix<3, 1, std::complex<double>> v = { g_1,g_2,g_3 };
		v = (1.0 / (g_1 * g_1 + g_2 * g_2 + g_3 * g_3)) * v;

		return vec3{
			(float)real(*v[0]),
			(float)real(*v[1]),
			(float)real(*v[2]) };
	}
};

template<typename func>
struct Surface {
public:
	Surface() {}
	Surface(func F, float s_max, float t_max) : _param_func(F), _s_max(s_max), _t_max(t_max) {}
	vec3 eval(float s, float t) { return _param_func(s, t); }
	vector<vec3> genPoints(int fineness);
	float sMax() { return _s_max; }
	float tMax() { return _t_max; }

protected:
	func _param_func;
	float _s_max, _t_max;
};

template<typename func>
vector<vec3> Surface<func>::genPoints(int fineness)
{
	vector<vec3> points = vector<vec3>(fineness);

	for (int i = 0; i < fineness; i++) {

		float s = _s_max * uniform_rand();
		float t = _t_max * uniform_rand();

		points[i] = eval(s, t);
	}

	return points;
}

/*Generate connected lattice of vertices.*/
Vertex** genLattice(int N, int L);

Vertex* genPolygon(int penis);


#endif