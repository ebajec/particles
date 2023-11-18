#pragma once
#ifndef POINT_CLOUD_H
#define POINT_CLOUD_H

#include <shapes.h>
#include <drawable.h>

#define POINT_ATTRIBUTES 2

class PointCloud : public Drawable<POINT_ATTRIBUTES> {
public:
	PointCloud() {}
	PointCloud(vector<vec3> points);
	template<typename paramFunc>
	PointCloud(Surface<paramFunc> S, int fineness);

	void set_color(vec3 color) { _color = color; }
	void reinitBuffer(GLenum usage, unsigned int attribute);
private:
	vector<vec3> _points;
	vec3 _color = { 1,1,1 };

	void _init();
	inline unsigned long _object_count();
	void _loadVBOs(float** attribute_buffers);
};

#include "point_cloud.hpp"

#endif


