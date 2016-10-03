#include "EventManager.h"
#include "WindowManager.h"
#include "GraphicsManager.h"
#include "glfw3.h"

#include <iostream>


EventManager::EventManager()
{
}


EventManager::~EventManager()
{
}

void EventManager::UpdateEvents()
{
	glfwPollEvents();
}

void EventManager::InitializeEvents()
{
	glfwSetKeyCallback(WindowManager::GetWindow(), &EventManager::EventCallback);
}

void EventManager::EventCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	
	if (key == GLFW_KEY_N && action == GLFW_PRESS)
		GraphicsManager::SetWireframeMode(!GraphicsManager::IsWireframeModeOn());
}
