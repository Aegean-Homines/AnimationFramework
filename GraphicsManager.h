#pragma once
#define GLM_FORCE_RADIANS

#include <unordered_map>
#include "ShaderProgram.h"
#include "VAO.h"
#include "Mesh.h"
#include "Types.h"
#include "Camera.h"
#include "SkeletonNode.h"

typedef std::unordered_map<MeshType, Mesh*> MeshMap;

class GraphicsManager
{
public:
	GraphicsManager();
	~GraphicsManager();

	static void Render();
	static void Update();
	static void ResetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
	static void InitializeGraphics();
	static void InitializeShaders();
	static void InitializeData();

	static bool IsWireframeModeOn() { return isWireframeModeOn; }
	static void SetWireframeMode(bool wireframeMode);

	static Mesh* GetMesh(MeshType type);
	static Camera& GetCamera() { return camera; }
	static int Width() { return width; }
	static void Width(int newWidth) { width = newWidth; }
	static int Height() { return height; }
	static void Height(int newHeight) { height = newHeight; }
private:
	static ShaderProgram simpleShader;
	static VAO vao;

	static bool isWireframeModeOn;
	static bool isAnimating;
	static double animationStartingTime;

	static MeshMap meshMap;

	static int width, height;
	static Camera camera;

	static SkeletonNode node;

	static void DrawGround(ShaderProgram& program);
};

