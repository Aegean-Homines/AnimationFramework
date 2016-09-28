#pragma once

struct GLFWwindow;

class WindowManager
{
private:
	static GLFWwindow * window;

public:
	WindowManager();
	~WindowManager();

	static void InitializeWindow(unsigned int width, unsigned int height, char* windowTitle);
	static GLFWwindow* GetWindow() { return window; }

	static void Cleanup();

};

