#include "WindowManager.h"
#include "glfw3.h"
#include <iostream>

GLFWwindow * WindowManager::window = NULL;

WindowManager::WindowManager()
{
}


WindowManager::~WindowManager()
{

}

void WindowManager::InitializeWindow(unsigned int width, unsigned int height, char* windowTitle)
{
	window = glfwCreateWindow(width, height, windowTitle, nullptr, nullptr);
	if (!window) {
		std::cout << "Window creation failed!" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
}

void WindowManager::Cleanup()
{
	glfwDestroyWindow(window);
	std::cout << "Window cleaned up." << std::endl;
}
