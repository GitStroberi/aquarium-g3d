#include "Cube.h"

void Cube::init(float len_x, float len_y, float len_z, const glm::vec3& pos, TEXSCALE ts)
{
	len_x /= 2.0f;
	len_y /= 2.0f;
	len_z /= 2.0f;

	this->position = pos;
	vertices.resize(36);

	int i = 0;
	vertices[i++] = { -len_x,-len_y,-len_z, 0.0f, 0.0f,-1.0f, 0.0f, 0.0f, }; // bottom-left
	vertices[i++] = { len_x, len_y,-len_z, 0.0f, 0.0f,-1.0f, 1.0f, 1.0f, }; // top-lright
	vertices[i++] = { len_x,-len_y,-len_z, 0.0f, 0.0f,-1.0f, 1.0f, 0.0f, }; // bottom-lright         
	vertices[i++] = { len_x, len_y,-len_z, 0.0f, 0.0f,-1.0f, 1.0f, 1.0f, }; // top-lright
	vertices[i++] = { -len_x,-len_y,-len_z, 0.0f, 0.0f,-1.0f, 0.0f, 0.0f, }; // bottom-left
	vertices[i++] = { -len_x, len_y,-len_z, 0.0f, 0.0f,-1.0f, 0.0f, 1.0f, }; // top-left

	vertices[i++] = { -len_x,-len_y, len_z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, }; // bottom-left
	vertices[i++] = { len_x,-len_y, len_z, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, }; // bottom-lright
	vertices[i++] = { len_x, len_y, len_z, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, }; // top-lright
	vertices[i++] = { len_x, len_y, len_z, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, }; // top-lright
	vertices[i++] = { -len_x, len_y, len_z, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, }; // top-left
	vertices[i++] = { -len_x,-len_y, len_z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, }; // bottom-left

	vertices[i++] = { -len_x, len_y, len_z,-1.0f, 0.0f, 0.0f, 1.0f, 0.0f, }; // top-lright
	vertices[i++] = { -len_x, len_y,-len_z,-1.0f, 0.0f, 0.0f, 1.0f, 1.0f, }; // top-left
	vertices[i++] = { -len_x,-len_y,-len_z,-1.0f, 0.0f, 0.0f, 0.0f, 1.0f, }; // bottom-left
	vertices[i++] = { -len_x,-len_y,-len_z,-1.0f, 0.0f, 0.0f, 0.0f, 1.0f, }; // bottom-left
	vertices[i++] = { -len_x,-len_y, len_z,-1.0f, 0.0f, 0.0f, 0.0f, 0.0f, }; // bottom-lright
	vertices[i++] = { -len_x, len_y, len_z,-1.0f, 0.0f, 0.0f, 1.0f, 0.0f, }; // top-lright

	vertices[i++] = { len_x, len_y, len_z, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, }; // top-left
	vertices[i++] = { len_x,-len_y,-len_z, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, }; // bottom-lright
	vertices[i++] = { len_x, len_y,-len_z, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, }; // top-lright         
	vertices[i++] = { len_x,-len_y,-len_z, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, }; // bottom-lright
	vertices[i++] = { len_x, len_y, len_z, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, }; // top-left
	vertices[i++] = { len_x,-len_y, len_z, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, }; // bottom-left     

	vertices[i++] = { -len_x,-len_y,-len_z, 0.0f,-1.0f, 0.0f, 0.0f, 1.0f, }; // top-lright
	vertices[i++] = { len_x,-len_y,-len_z, 0.0f,-1.0f, 0.0f, 1.0f, 1.0f, }; // top-left
	vertices[i++] = { len_x,-len_y, len_z, 0.0f,-1.0f, 0.0f, 1.0f, 0.0f, }; // bottom-left
	vertices[i++] = { len_x,-len_y, len_z, 0.0f,-1.0f, 0.0f, 1.0f, 0.0f, }; // bottom-left
	vertices[i++] = { -len_x,-len_y, len_z, 0.0f,-1.0f, 0.0f, 0.0f, 0.0f, }; // bottom-lright
	vertices[i++] = { -len_x,-len_y,-len_z, 0.0f,-1.0f, 0.0f, 0.0f, 1.0f, }; // top-lright

	vertices[i++] = { -len_x, len_y,-len_z, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, }; // top-left
	vertices[i++] = { len_x, len_y, len_z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, }; // bottom-lright
	vertices[i++] = { len_x, len_y,-len_z, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, }; // top-lright     
	vertices[i++] = { len_x, len_y, len_z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, }; // bottom-lright
	vertices[i++] = { -len_x, len_y,-len_z, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, }; // top-left
	vertices[i++] = { -len_x, len_y, len_z, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f }; // bottom-left

	if (ts == TS_SCALE)
	{
		for (int i = 0; i < 12; i++)
		{
			vertices[i].texture.x *= len_x;
			vertices[i].texture.y *= len_y;
		}
		for (int i = 12; i < 24; i++)
		{
			vertices[i].texture.x *= len_z;
			vertices[i].texture.y *= len_y;
		}
		for (int i = 24; i < 36; i++)
		{
			vertices[i].texture.x *= len_x;
			vertices[i].texture.y *= len_z;
		}
	}

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

Cube::Cube(const glm::vec3& firstVertex, const glm::vec3& secondVertex, TEXSCALE ts)
{
	glm::vec3 lowerVertex = {
			std::min(firstVertex.x, secondVertex.x),
			std::min(firstVertex.y, secondVertex.y),
			std::min(firstVertex.z, secondVertex.z)
	};
	glm::vec3 higherVertex = {
		std::max(firstVertex.x, secondVertex.x),
		std::max(firstVertex.y, secondVertex.y),
		std::max(firstVertex.z, secondVertex.z)
	};

	glm::vec3 pos = (higherVertex + lowerVertex) * 0.5f;
	glm::vec3 dimensions = higherVertex - lowerVertex;

	init(dimensions.x, dimensions.y, dimensions.z, pos, ts);
}

Cube::Cube(float len_x, float len_y, float len_z, const glm::vec3& pos, TEXSCALE ts)
{
	init(len_x, len_y, len_z, pos, ts);
}
