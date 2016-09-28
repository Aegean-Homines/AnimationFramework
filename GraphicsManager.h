#pragma once

#include "ShaderProgram.h"
#include "VAO.h"

class GraphicsManager
{
public:
	GraphicsManager();
	~GraphicsManager();

	static void Render();
	static void ResetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
	static void InitializeGraphics();
	static void InitializeShaders();
	static void InitializeData();
private:
	static ShaderProgram denemeShader;
	static VAO vao;
};

