#pragma once

#include <vector>

using std::vector;

struct GLFWwindow;

enum MouseButton {
	LEFT,
	MIDDLE,
	RIGHT,
	LAST
};

class EventManager
{
public:
	EventManager();
	~EventManager();
	static void UpdateEvents();
	static void InitializeEvents();
	static bool IsKeyPressed(int key);
	static bool IsKeyReleased(int key);
	static bool IsKeyTriggered(int key);
	static double MouseDeltaX() { return mouseDeltaX; }
	static void MouseDeltaX(double newMouseDeltaX) { mouseDeltaX = newMouseDeltaX; }
	static double MouseDeltaY() { return mouseDeltaY; }
	static void MouseDeltaY(double newMouseDeltaY) { mouseDeltaY = newMouseDeltaY; }
	static bool IsMouseButtonPressed(MouseButton button) { return mouseButtons[button]; }
private:
	static void KeyboardEventCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void MouseEventCallback(GLFWwindow* window, double xpos, double ypos);
	static void MouseButtonEventCallback(GLFWwindow* window, int button, int action, int mods);
	static vector<bool> keys;
	static vector<bool> prevKeys;

	static double mouseDeltaX;
	static double mouseDeltaY;
	static double lastX;
	static double lastY;

	static bool mouseButtons[LAST];
};

