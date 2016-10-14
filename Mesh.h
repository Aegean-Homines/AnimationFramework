#pragma once

#include <glm.hpp>
#include <vector>
#include "VAO.h"
#include "ShaderProgram.h"

using std::vector;
using glm::vec3;
using glm::mat4;

struct Vertex {
	vec3 position;
	vec3 color;

	Vertex() { position = vec3(0.0f); color = vec3(1.0f); }
	Vertex(glm::vec3 position, glm::vec3 color) : position(position), color(color) {};
};

class Mesh
{
public:
	vector<Vertex> vertices;
	vector<GLuint> indices;
	GLuint drawType;

	Mesh(vector<Vertex> const & vertices, vector<GLuint> const & indices, GLuint drawType);
	
	// HELPER METHODS
	void Draw(ShaderProgram program);
	void BufferNewData(vector<Vertex> const & newVertices);
private:
	VAO vao;
	void InitializeMesh();

};

