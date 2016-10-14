#define GLM_FORCE_RADIANS

#include "Mesh.h"
#include <glfw3.h>

Mesh::Mesh(vector<Vertex> const & vertices, vector<GLuint> const & indices, GLuint drawType)
	:
	vertices(vertices), indices(indices), drawType(drawType)
{
	InitializeMesh();
}

void Mesh::Draw(ShaderProgram program) 
{
	// Draw mesh
	vao.Bind();
	glDrawElements(drawType, indices.size(), GL_UNSIGNED_INT, 0);
	vao.Unbind();
}

void Mesh::BufferNewData(vector<Vertex> const & newVertices)
{
	vao.Bind();
	glBufferData(GL_ARRAY_BUFFER, newVertices.size() * sizeof(Vertex), &newVertices[0], GL_STATIC_DRAW);
	vao.Unbind();
}

void Mesh::InitializeMesh()
{
	GLuint vbo, ebo;
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	vao.Generate();
	vao.Bind();

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)offsetof(Vertex, color));
	vao.Unbind();
}
