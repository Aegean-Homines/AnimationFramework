#include "EventManager.h"
#include "WindowManager.h"
#include "GraphicsManager.h"
#include <glfw3.h>
#include <iostream>

vector<bool> EventManager::keys;

double EventManager::mouseDeltaX;
double EventManager::mouseDeltaY;
double EventManager::lastX;
double EventManager::lastY;

bool EventManager::mouseButtons[LAST];

EventManager::EventManager()
{
}

EventManager::~EventManager()
{
}

void EventManager::UpdateEvents()
{
	mouseDeltaX = mouseDeltaY = 0;
	glfwPollEvents();
}

void EventManager::InitializeEvents()
{
	glfwSetKeyCallback(WindowManager::GetWindow(), &EventManager::KeyboardEventCallback);
	glfwSetCursorPosCallback(WindowManager::GetWindow(), &EventManager::MouseEventCallback);
	glfwSetMouseButtonCallback(WindowManager::GetWindow(), &EventManager::MouseButtonEventCallback);
	keys.resize(GLFW_KEY_LAST, false);

	mouseDeltaX = 0.0f;
	mouseDeltaY = 0.0f;
	lastX = static_cast<double>(GraphicsManager::Width()) / 2;
	lastY = static_cast<double>(GraphicsManager::Height()) / 2;

	memset(mouseButtons, false, LAST * sizeof(bool));
}

bool EventManager::IsKeyPressed(int key)
{
	return keys[key];
}

bool EventManager::IsKeyReleased(int key)
{
	return !keys[key];
}

void EventManager::KeyboardEventCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS)
		keys[key] = true;
	if (action == GLFW_RELEASE)
		keys[key] = false;
}

void EventManager::MouseEventCallback(GLFWwindow* window, double xpos, double ypos)
{
	mouseDeltaX = xpos - lastX;
	mouseDeltaY = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

}

void EventManager::MouseButtonEventCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) {
		if (button == GLFW_MOUSE_BUTTON_LEFT)
			mouseButtons[LEFT] = true;
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
			mouseButtons[RIGHT] = true;
		if (button == GLFW_MOUSE_BUTTON_MIDDLE)
			mouseButtons[MIDDLE] = true;
	}

	if (action == GLFW_RELEASE) {
		if (button == GLFW_MOUSE_BUTTON_LEFT)
			mouseButtons[LEFT] = false;
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
			mouseButtons[RIGHT] = false;
		if (button == GLFW_MOUSE_BUTTON_MIDDLE)
			mouseButtons[MIDDLE] = false;
	}
}
