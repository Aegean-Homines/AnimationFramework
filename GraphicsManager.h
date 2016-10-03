#pragma once

#include <vector>
#include "ShaderProgram.h"
#include "VAO.h"
#include "Mesh.h"

enum MeshType {
	TRIANGLE,
	CUBE
};

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

	static bool IsWireframeModeOn() { return isWireframeModeOn; }
	static void SetWireframeMode(bool wireframeMode);
private:
	static ShaderProgram denemeShader;
	static VAO vao;

	static bool isWireframeModeOn;
	static std::vector<Mesh> meshList;

	static int width, height;
};

