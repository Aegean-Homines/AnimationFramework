#pragma once

#include "glew.h"
class VAO
{
public:
	VAO();
	~VAO();

	void Generate();
	void Bind();
	void Unbind();
private:
	GLuint vaoID;
};

