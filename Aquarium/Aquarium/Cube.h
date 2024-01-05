#pragma once
#include "Mesh.h"

class Cube : public Mesh {

protected:
	void init(float len_x, float len_y, float len_z, const glm::vec3& pos, TEXSCALE ts);

public:
	Cube(const glm::vec3& firstVertex, const glm::vec3& secondVertex, TEXSCALE ts = TS_NO_SCALE);

	Cube(float len_x, float len_y, float len_z, const glm::vec3& pos = { 0,0,0 }, TEXSCALE ts = TS_NO_SCALE);
};