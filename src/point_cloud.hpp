#ifndef POINT_CLOUD_HPP
#define POINT_CLOUD_HPP
#include "point_cloud.h"

PointCloud::PointCloud(vector<vec3> points) : Drawable<POINT_ATTRIBUTES>() {
	_init();
	this->_points = points;
}

template<typename paramFunc>
PointCloud::PointCloud(Surface<paramFunc> S, int fineness) : Drawable<POINT_ATTRIBUTES>()
{
	_init();
	this->_points = S.genPoints(fineness);
}


void PointCloud::reinitBuffer(GLenum usage, unsigned int attribute)
{

}

void PointCloud::_init()
{
	_draw_mode = GL_POINTS;

	_layout_map[0] = 0;
	_layout_map[1] = 2;

	_primitive_sizes[0] = 3;
	_primitive_sizes[1] = 3;
}

unsigned long PointCloud::_object_count()
{
	return _points.size();
}

void PointCloud::_loadVBOs(float** attribute_buffers)
{
	int counter = 0;
	float* color_data = _color.data();

	for (vec3 v : _points) {
		float* pos_data = v.data();
		copy(pos_data, pos_data + 3, attribute_buffers[0] + 3 * counter);
		copy(color_data, color_data + 3, attribute_buffers[1] + 3 * counter);
		counter++;
	}
}


#endif