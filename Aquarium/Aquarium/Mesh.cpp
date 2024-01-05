#include "Mesh.h"

void Mesh::init(const std::vector<Vertex>& verts, const glm::vec3& pos)
{
	vertices = verts;

	if (vertices.empty())
		return;

	// plane VAO
	glGenVertexArrays(1, &meshVAO);
	glGenBuffers(1, &meshVBO);
	// fill buffer
	glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	// link vertex attributes
	glBindVertexArray(meshVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normals));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Mesh::Mesh(const std::vector<Vertex>& verts, const glm::vec3& pos)
{
	init(verts, pos);
}

Mesh::~Mesh()
{
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteVertexArrays(1, &meshVAO);
	glDeleteBuffers(1, &meshVBO);
}

void Mesh::setDiffuseTextureId(unsigned dti)
{
	diffuseTextureId = dti;
}

unsigned Mesh::getDiffuseTextureId() const
{
	return diffuseTextureId;
}

glm::vec3 Mesh::getPosition() const
{
	return position;
}

void Mesh::setPosition(glm::vec3 pos)
{
	position = pos;
}

void Mesh::applyModelTransform(const glm::mat4& model)
{
	glm::mat4 normalModelMatrix = glm::transpose(glm::inverse(model));

	for (auto& vertice : vertices)
	{
		//Coord
		auto vertexPos4 = glm::vec4(vertice.position, 1.0f);
		vertexPos4 = model * vertexPos4;
		vertice.position = vertexPos4;

		//Normal
		auto vertexNormal4 = glm::vec4(vertice.normals, 1.0f);
		vertexNormal4 = normalModelMatrix * vertexNormal4;
		vertice.normals = glm::normalize(vertexNormal4);
	}

	glBindVertexArray(meshVAO);
	glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}

double Mesh::getDistanceToCamera(glm::vec3 cameraPos) const
{
	return glm::distance(cameraPos, position);
}

void Mesh::renderMesh() const
{
	glBindVertexArray(meshVAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);
}

void Mesh::renderBasic(const Shader& shader, const glm::vec3& offsetPosition) const
{
	auto model = glm::mat4();
	model = glm::translate(model, this->getPosition() + offsetPosition);
	shader.SetMat4("model", model);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->getDiffuseTextureId());
	this->renderMesh();
}

void Mesh::renderCustomModel(const Shader& shader, const glm::mat4& model) const
{
	shader.SetMat4("model", model);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->getDiffuseTextureId());
	this->renderMesh();
}
