#include "VAO.h"
#include "glew.h"

VAO::VAO()
{
}


VAO::~VAO()
{
}

void VAO::Generate()
{
	glGenVertexArrays(1, &vaoID);
}

void VAO::Bind()
{
	glBindVertexArray(vaoID);
}

void VAO::Unbind()
{
	glBindVertexArray(0);
}
