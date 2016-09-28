#pragma once

struct GLFWwindow;

class EventManager
{
public:
	EventManager();
	~EventManager();

	static void UpdateEvents();

	static void InitializeEvents();
private:
	static void EventCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
};

