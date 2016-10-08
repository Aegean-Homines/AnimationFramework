#define GLEW_STATIC
#define GLM_FORCE_RADIANS

#include "EngineHelper.h"
#include "WindowManager.h"
#include "EventManager.h"
#include "GraphicsManager.h"
#include "ModelManager.h"

#include <iostream>

#define OPENGL_MAJOR_VERSION 3
#define OPENGL_MINOR_VERSION 3
#define INITIAL_WINDOW_WIDTH 1024
#define INITIAL_WINDOW_HEIGHT 768
#define WINDOW_NAME "AnimationClassHomework"

using std::string;

const string ModelMainDirectory = "Models/";
const string TadModelName = "Tad";
const string FbxExtension = ".fbx";


void main(){
	EngineHelper::InitializeGLFW(OPENGL_MAJOR_VERSION, OPENGL_MINOR_VERSION, false);
	WindowManager::InitializeWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, WINDOW_NAME);
	EventManager::InitializeEvents();
	EngineHelper::InitializeGlew();
	GraphicsManager::InitializeGraphics();
	GraphicsManager::InitializeShaders();
	GraphicsManager::InitializeData();
	ModelManager* manager = ModelManager::Instance();
	manager->CreateFbxManager();
	manager->CreateFbxScene(TadModelName.c_str(), (ModelMainDirectory + TadModelName + FbxExtension).c_str());
	//manager->PrintTree();

	EngineHelper::Update();

	EngineHelper::Exit();

}