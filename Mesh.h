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

	Vertex(glm::vec3 position, glm::vec3 color) : position(position), color(color) {};
};

class Mesh
{
public:
	vector<Vertex> vertices;
	vector<GLuint> indices;

	Mesh(vector<Vertex> const & vertices, vector<GLuint> const & indices);
	void Draw(ShaderProgram program);
private:
	VAO vao;
	void InitializeMesh();
};

