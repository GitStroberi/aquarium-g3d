#pragma once
#include <vec3.hpp>

class IRenderable
{
public:
	virtual double getDistanceToCamera(glm::vec3 cameraPos) const = 0;
	virtual void renderMesh() const = 0;
	virtual glm::vec3 getPosition() const = 0;
	virtual unsigned int getDiffuseTextureId() const = 0;
};

class IRendComparator
{
public:
	glm::vec3 camPos;
	bool operator()(const IRenderable* a, const IRenderable* b) const
	{
		double closestA = a->getDistanceToCamera(camPos);
		double closestB = b->getDistanceToCamera(camPos);
		return closestB < closestA;
	}
};