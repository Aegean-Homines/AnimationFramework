#define GLM_FORCE_RADIANS

#include "Mesh.h"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include <glfw3.h>

#define PI 3.14159265359f

const float angleMultiplication = (PI / 180.0f);

Mesh::Mesh(vector<Vertex> const & vertices, vector<GLuint> const & indices):
	vertices(vertices), indices(indices)
{
	translation = vec3(0.0f, 0.0f, 0.0f);
	scale = vec3(1.0f, 1.0f, 1.0f);
	rotation = vec3(0.0f, 0.0f, 1.0f);

	InitializeMesh();
}

void Mesh::Draw(ShaderProgram program)
{
	glm::mat4 transform;
	transform = glm::translate(transform, this->translation);
	transform = glm::rotate(transform, rotation.x * angleMultiplication, vec3(1.0f, 0.0f, 0.0f));
	transform = glm::rotate(transform, rotation.y * angleMultiplication, vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, rotation.z * angleMultiplication, vec3(0.0f, 0.0f, 1.0f));
	//transform = glm::rotate(transform, (GLfloat)glfwGetTime() * 40.0f, rotation);
	transform = glm::scale(transform, scale);

	GLint transformLocation = glGetUniformLocation(program.program, "Transform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));

	// Draw mesh
	vao.Bind();
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
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
