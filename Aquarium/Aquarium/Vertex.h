#pragma once

#include <glm.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normals;
	glm::vec2 texture;

	Vertex(float x = 0.0, float y = 0.0, float z = 0.0, float n1 = 0.0, float n2 = 0.0, float n3 = 0.0, float tx = 0.0, float ty = 0.0) :
		position(x, y, z), normals(n1, n2, n3), texture(tx, ty) {}
};


enum TEXSCALE
{
	TS_NO_SCALE,
	TS_SCALE
};