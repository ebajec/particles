#include "camera.h"

Camera::Camera(
	vec3 normal,
	vec3 pos,
	int w_screen,
	int h_screen,
	GLfloat FOV,
	GLfloat far)
	:
	_w_screen(w_screen),
	_h_screen(h_screen),
	_near_dist(1 / tan(FOV / 2)),
	_far_dist(far),
	_pos(pos)
{	
	//generate orthonormal, right-handed basis for camera coordinates, with Z
	//as the normal vector. xyz
	normal = normalize(normal);
	basis[2] = normal;
	basis[0] = cross(normal, vec3({ 0,1,0 }));
	basis[1] = cross(basis[0], basis[2]);
	basis[0] = normalize(basis[0]);
	basis[1] = normalize(basis[1]);

	change_of_basis = inv(basis[0] | basis[1] | basis[2]);

	float screen_ratio = (float)_h_screen / (float)_w_screen;
	_proj = {
		screen_ratio,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	_world = mat4(mat3::id() | -1 * (_pos - basis[2] * _near_dist));

	_view = mat4(change_of_basis);
	_model_yaw = mat4{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	_model_pitch = mat4{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
}

void Camera::connectUniforms(const ShaderProgram& shader)
{
	glUniform1f(shader.getUniform("near_dist"), _near_dist);
	glUniform1f(shader.getUniform("far_dist"), _far_dist);
	glUniformMatrix4fv(shader.getUniform("cam_projection"), 1, GL_FALSE, _proj.data());
	glUniformMatrix4fv(shader.getUniform("cam_modelview"), 1, GL_TRUE, (_view * _model_pitch * _model_yaw * _world).data());
}

void Camera::rotate(float pitch, float yaw)
{
	if (abs(pitch) > PI) return;
	_model_yaw = mat4(rotatexz<GLfloat>(yaw)) * _model_yaw;
	_model_pitch = mat4(rot_axis(basis[0], -pitch)) * _model_pitch;
}

void Camera::translate(vec3 delta) {
	_pos = _pos + change_of_basis * mat3(_model_yaw) * delta;
	_updateViewMat();
}

void Camera::reset()
{
	_pos = { 0,0,0 };
	_model_yaw = mat4{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	_model_pitch = mat4{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	_updateViewMat();
}

