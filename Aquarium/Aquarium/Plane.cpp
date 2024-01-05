#include "Plane.h"

void Plane::initCommon()
{
	glGenVertexArrays(1, &meshVAO);
	glGenBuffers(1, &meshVBO);
	glBindVertexArray(meshVAO);
	glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, normals));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, texture));
	glBindVertexArray(0);
}

void Plane::init(float len, float wid, const glm::vec3& pos, TEXSCALE ts, ORIENTATION ori)
{
	length = len;
	width = wid;

	len /= 2.0f;
	wid /= 2.0f;
	position = pos;

	vertices.resize(6);

	vertices[0] = { len,0, wid,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f };
	vertices[1] = { -len,0, wid,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f };
	vertices[2] = { -len,0,-wid,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f };

	vertices[3] = { len,0, wid,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f };
	vertices[4] = { -len,0,-wid,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f };
	vertices[5] = { len,0,-wid,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f };

	if (ts == TS_SCALE)
	{
		for (int i = 0; i <= 5; i++)
		{
			vertices[i].texture.x *= len;
			vertices[i].texture.y *= wid;
		}
	}

	if (ori == OR_XY)
	{
		for (int i = 0; i < 6; i++)
			std::swap(vertices[i].position.y, vertices[i].position.z);
	}
	if (ori == OR_ZY)
	{
		for (int i = 0; i < 6; i++) {
			std::swap(vertices[i].position.x, vertices[i].position.y);
		}
	}

	// plane VAO
	initCommon();
}

void Plane::init(glm::vec3 fs, glm::vec3 sc, TEXSCALE ts)
{
	glm::vec3 mn(std::min(fs.x, sc.x), std::min(fs.y, sc.y), std::min(fs.z, sc.z));
	glm::vec3 mx(std::max(fs.x, sc.x), std::max(fs.y, sc.y), std::max(fs.z, sc.z));

	glm::vec3 a, b, c, d;
	glm::vec3 normal;

	if (fs.x == sc.x)
	{
		a = { mn.x, mx.y, mn.z };
		b = mx;
		c = { mn.x, mn.y, mx.z };
		d = mn;

		normal = { 1,0,0 };
	}
	else if (fs.y == sc.y)
	{
		a = { mn.x, mn.y, mx.z };
		b = mx;
		c = { mx.x, mn.y, mn.z };
		d = mn;

		normal = { 0,1,0 };
	}
	else if (fs.z == sc.z)
	{
		a = { mn.x, mx.y, mn.z };
		b = mx;
		c = { mx.x, mn.y, mn.z };
		d = mn;

		normal = { 0,0,1 };
	}
	else
	{
		std::cerr << "INVALID PLANE";
		exit(1);
	}

	length = glm::distance(a, b);
	width = glm::distance(a, d);

	vertices.resize(6);
	vertices[0] = { a.x, a.y, a.z,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f };
	vertices[1] = { d.x, d.y, d.z,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	vertices[2] = { c.x, c.y, c.z,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

	vertices[3] = { a.x, a.y, a.z,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f };
	vertices[4] = { c.x, c.y, c.z,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	vertices[5] = { b.x, b.y, b.z,  0.0f, 0.0f, 0.0f,  1.0f, 1.0f };

	for (auto& vertex : vertices)
		vertex.normals = normal;

	if (ts == TS_SCALE)
	{
		for (int i = 0; i <= 5; i++)
		{
			vertices[i].texture.x *= length;
			vertices[i].texture.y *= width;
		}
	}

	initCommon();
}

void Plane::updateBillboardRotation(glm::vec3 right, glm::vec3 up)
{
	glm::vec3 a = (-right * length + up * width) * 0.5f;
	glm::vec3 b = (+right * length + up * width) * 0.5f;
	glm::vec3 c = (+right * length - up * width) * 0.5f;
	glm::vec3 d = (-right * length - up * width) * 0.5f;

	vertices[0].position = a; vertices[0].texture = { 0,1 };
	vertices[1].position = b; vertices[1].texture = { 1,1 };
	vertices[2].position = c; vertices[2].texture = { 1,0 };
	vertices[3].position = c; vertices[3].texture = { 1,0 };
	vertices[4].position = d; vertices[4].texture = { 0,0 };
	vertices[5].position = a; vertices[5].texture = { 0,1 };

	glBindVertexArray(meshVAO);
	glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}
