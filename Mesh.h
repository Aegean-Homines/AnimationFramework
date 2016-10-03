#pragma once

#include <glm.hpp>
#include <glew.h>
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

	vec3 Translation() const { return translation; }
	void Translation(vec3 const & newTranslation) { translation = newTranslation; }
	vec3 Scale() const { return scale; }
	void Scale(vec3 const & newScale) { scale = newScale; }
	vec3 Rotation() const { return rotation; }
	void Rotation(vec3 const & newRotation) { rotation = newRotation; }
private:
	VAO vao;
	void InitializeMesh();

	vec3 translation, scale, rotation;
};

