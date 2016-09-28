#pragma once

class GraphicsManager
{
public:
	GraphicsManager();
	~GraphicsManager();

	static void Render();
	static void ResetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
	static void InitializeGraphics();
};

