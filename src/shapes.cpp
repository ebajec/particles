#include "shapes.h"
#include <iostream>


Vertex** genLattice(int N, int L) {

	Vertex** layers = new Vertex * [L];
	mat3 R = rotatexy<GLfloat>(2 * PI / N);
	vec3 start_position = { 1,0,0 };
	//direction of first edge in a layer.
	vec3 start_angle = (R ^ 2) * start_position;

	//set up origin
	layers[0] = new Vertex({ 0,0,0 }, { 0,0,1 });

	//make the position of the first vertex in each layer sit on x-axis
	for (int n = 1; n < L + 1; n++) {
		layers[n] = new Vertex[N * n];
		for (int i = 0; i < N * n; i++) {
			layers[n][i] = Vertex({ 0,0,0 }, { 0,0,1 });
		}
		layers[n][0].position = layers[n - 1][0].position + start_position;
	}

	//generate positions for other points in each layer
	for (int n = 1; n <= L; n++) {
		vec3 edge_dir = start_angle;

		for (int i = 1; i < N * n; i++) {
			Vertex* point_a = layers[n] + i;
			Vertex* point_b = layers[n] + i - 1;
			point_a->position = (point_b)->position + edge_dir;

			if (i % n == 0) {
				edge_dir = R * edge_dir;
			}
		}
	}


	//connect origin point to layer 1
	for (int i = 0; i < N; i++) {
		layers[0]->connect(layers[1] + i);
	}

	//connect convex hull of each layer
	for (int n = 1; n <= L; n++) {
		for (int i = 1; i < N * n; i++) {
			(layers[n] + i)->connect(layers[n] + i - 1);
		}
		(layers[n])->connect(layers[n] + N * n - 1);
	}

	//connect subsequent layers
	for (int n = 1; n < L; n++) {
		for (int i = 0; i < N * n; i++) {
			Vertex* point = (layers[n] + i);

			if (i % n == 0) {
				int c_i = i + i / n;
				int next_layer_size = N * (n + 1);
				point->connect(
					layers[n + 1] + modulo(c_i - 1, next_layer_size),
					layers[n + 1] + modulo(c_i, next_layer_size),
					layers[n + 1] + modulo(c_i + 1, next_layer_size)
				);
			}
			else {
				int c_i = i + (i - i % n) / n;
				point->connect(
					layers[n + 1] + c_i,
					layers[n + 1] + c_i + 1
				);
			}
		}
	}

	return layers;
}

Vertex* genPolygon(int size) {
	int N = 6;
	Vertex** lattice_front = genLattice(N, size);
	Vertex** lattice_back = genLattice(N, size + 1);

	for (int n = 0; n <= size; n++) {
		for (int i = 0; i < N * n; i++) {
			lattice_front[n][i].position = lattice_front[n][i].position + ROOT3OVER2 * (size - n + 1) * vec3 { 0, 0, 1 };
			lattice_back[n][i].position = lattice_back[n][i].position - ROOT3OVER2 * (size - n + 1) * vec3 { 0, 0, 1 };
		}
	}

	lattice_front[0][0].position = vec3{ 0, 0, ROOT3OVER2 * (size + 1) };
	lattice_back[0][0].position = -1 * vec3{ 0, 0, ROOT3OVER2* (size + 1) };

	int n = size;
	for (int i = 0; i < N * n; i++) {
		Vertex* point = (lattice_front[n] + i);

		if (i % n == 0) {
			int c_i = i + i / size;
			int next_layer_size = N * (n + 1);
			point->connect(
				lattice_back[n + 1] + modulo(c_i - 1, next_layer_size),
				lattice_back[n + 1] + modulo(c_i, next_layer_size),
				lattice_back[n + 1] + modulo(c_i + 1, next_layer_size)
			);
		}
		else {
			int c_i = i + (i - i % n) / n;
			point->connect(
				lattice_back[size + 1] + c_i,
				lattice_back[size + 1] + c_i + 1
			);
		}
	}

	return lattice_front[0];

}





