#include "GraphicsManager.h"
#include "WindowManager.h"
#include "glfw3.h"


GraphicsManager::GraphicsManager()
{
}


GraphicsManager::~GraphicsManager()
{
}

void GraphicsManager::Render()
{

	glClearColor(0.2f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glfwSwapBuffers(WindowManager::GetWindow());
}

void GraphicsManager::ResetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	glViewport(x, y, width, height);
}

void GraphicsManager::InitializeGraphics()
{
	int width, height;
	glfwGetFramebufferSize(WindowManager::GetWindow(), &width, &height);

	ResetViewport(0, 0, width, height);
}
