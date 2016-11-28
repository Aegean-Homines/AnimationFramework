#pragma once
#define GLM_FORCE_RADIANS

#include <unordered_map>
#include "ShaderProgram.h"
#include "VAO.h"
#include "Mesh.h"
#include "Types.h"
#include "Camera.h"
#include "SkeletonNode.h"
#include "TargetObject.h"

typedef std::unordered_map<MeshType, Mesh*> MeshMap;

// Simple graphics manager
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
	static float DeltaTime() { return deltaTime; }
	static void DeltaTime(float val) { deltaTime = val; }
	static float ElapseTime() { return elapseTime; }
	static void ElapseTime(float val) { elapseTime = val; }
private:
	static ShaderProgram simpleShader;
	static VAO vao;

	static bool isWireframeModeOn;
	static bool isAnimating;
	static double animationStartingTime;

	static MeshMap meshMap;

	static int width, height;

	static float animationSpeed;
	static Camera camera;

	static SkeletonNode node;

	static void DrawGround(ShaderProgram& program);
	static void DrawTarget(ShaderProgram& program);

	static float deltaTime;
	static float elapseTime;
};

