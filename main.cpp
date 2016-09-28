#define GLEW_STATIC

#include "EngineHelper.h"
#include "WindowManager.h"
#include "EventManager.h"
#include "GraphicsManager.h"

#include <iostream>

void main(){
	EngineHelper::InitializeGLFW(3, 3, false);
	WindowManager::InitializeWindow(600, 800, "Egeboi");
	EventManager::InitializeEvents();
	EngineHelper::InitializeGlew();
	GraphicsManager::InitializeGraphics();

	// THIS CODE WILL BE MOVED LATER
	
	EngineHelper::Update();

	EngineHelper::Exit();

}