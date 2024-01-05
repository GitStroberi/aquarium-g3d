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
#include <vector>

#include "IRenderable.h"
#include "Vertex.h"
#include "Shader.h"

class Mesh : public IRenderable
{
protected:
	unsigned int meshVAO = 0;
	unsigned int meshVBO = 0;
	std::vector<Vertex> vertices;
	glm::vec3 position;
	unsigned int diffuseTextureId = 0;

	void init(const std::vector<Vertex>& verts, const glm::vec3& pos = { 0,0,0 });

public:
	Mesh(const std::vector<Vertex>& verts = {}, const glm::vec3& pos = { 0,0,0 });

	virtual ~Mesh();

	void setDiffuseTextureId(unsigned int dti);

	unsigned getDiffuseTextureId() const override;

	glm::vec3 getPosition() const override;

	virtual void setPosition(glm::vec3 pos);

	virtual void applyModelTransform(const glm::mat4& model);

	double getDistanceToCamera(glm::vec3 cameraPos) const override;

	void renderMesh() const override;

	void renderBasic(const Shader& shader, const glm::vec3& offsetPosition = { 0,0,0 }) const;

	void renderCustomModel(const Shader& shader, const glm::mat4& model) const;
};