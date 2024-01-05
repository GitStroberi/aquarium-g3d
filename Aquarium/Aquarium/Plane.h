#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <math.h> 
#include <GL/glew.h>

#define GLM_FORCE_CTOR_INIT 
#include <GLM.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include "Mesh.h"
#include "Vertex.h"

enum ORIENTATION
{
	OR_XZ,
	OR_XY,
	OR_ZY
};

class Plane : public Mesh {
protected:
	float length;
	float width;

	void initCommon();
	void init(float len, float wid, const glm::vec3& pos, TEXSCALE ts, ORIENTATION ori);

	void init(glm::vec3 fs, glm::vec3 sc, TEXSCALE ts);

public:
	Plane(float len, float wid, const glm::vec3& pos, TEXSCALE ts = TS_NO_SCALE, ORIENTATION ori = OR_XZ)
	{
		init(len, wid, pos, ts, ori);
	}

	Plane(glm::vec3 fs, glm::vec3 sc, TEXSCALE ts)
	{
		init(fs, sc, ts);
	}

	void updateBillboardRotation(glm::vec3 right, glm::vec3 up);
};