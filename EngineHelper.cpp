#include "EngineHelper.h"
#include "WindowManager.h"
#include "GraphicsManager.h"
#include "EventManager.h"
#include <glew.h>
#include <glfw3.h>

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
	glfwWindowHint(GLFW_REFRESH_RATE, 60);
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
		double currentFrame = glfwGetTime();
		EventManager::UpdateEvents();
		WindowManager::Update();
		GraphicsManager::Update();
		GraphicsManager::Render();

		lastFrame = glfwGetTime();
		/*do
		{
			
		} while (lastFrame - currentFrame < (1.0f / 60.0f));*/

		deltaTime = lastFrame - currentFrame;
	}
}

void EngineHelper::Exit()
{
	WindowManager::Cleanup();
	glfwTerminate();

	std::cout << "Everything in the engine is cleaned up!" << std::endl;
}

double EngineHelper::lastFrame = 0.0f;
double EngineHelper::deltaTime = 0.0f;
