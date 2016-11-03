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
const string RunningModelName = "running_inPlace";
const string FbxExtension = ".fbx";


void main(){
	string CurrentModelName = RunningModelName;
	EngineHelper::InitializeGLFW(OPENGL_MAJOR_VERSION, OPENGL_MINOR_VERSION, false);
	WindowManager::InitializeWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, WINDOW_NAME);
	EventManager::InitializeEvents();
	EngineHelper::InitializeGlew();
	GraphicsManager::InitializeGraphics();
	GraphicsManager::InitializeShaders();
	GraphicsManager::InitializeData();
	ModelManager* manager = ModelManager::Instance();
	manager->CreateFbxManager();
	manager->CreateFbxScene(CurrentModelName.c_str(), (ModelMainDirectory + CurrentModelName + FbxExtension).c_str());
	manager->CreateTree(CurrentModelName);
	manager->CurrentAnimation(CurrentModelName);
	manager->ScaleSkeleton(CurrentModelName, 0.025f);
	manager->ChangeSkeletonColor(vec3(0.2f, 0.1f, 1.0f));
	EngineHelper::Update();

	EngineHelper::Exit();

}