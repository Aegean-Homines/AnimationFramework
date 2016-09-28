#include "EngineHelper.h"
#include "WindowManager.h"
#include "GraphicsManager.h"
#include "EventManager.h"
#include "glew.h"
#include "glfw3.h"

#include <iostream>

EngineHelper::EngineHelper()
{
}


EngineHelper::~EngineHelper()
{
}

void EngineHelper::InitializeGLFW(unsigned int openGLMajorVersion, unsigned int openGLMinorVersion, bool isResizable)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, openGLMajorVersion);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, openGLMinorVersion);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, isResizable);
}

void EngineHelper::InitializeGlew()
{
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW Initialization failed." << std::endl;
		exit(EXIT_FAILURE);
	}
}

void EngineHelper::Update()
{
	while (!glfwWindowShouldClose(WindowManager::GetWindow())) {
		EventManager::UpdateEvents();
		GraphicsManager::Render();
	}
}

void EngineHelper::Exit()
{
	WindowManager::Cleanup();
	glfwTerminate();

	std::cout << "Everything in the engine is cleaned up!" << std::endl;
}