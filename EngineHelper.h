#pragma once

class EngineHelper
{
public:
	EngineHelper();
	~EngineHelper();

	static void InitializeGLFW(unsigned int openGLMajorVersion, unsigned int openGLMinorVersion, bool isResizable);
	static void InitializeGlew();
	static void Update();
	static void Exit();
	static double DeltaTime() { return deltaTime; }
	static void DeltaTime(double newDeltaTime) { deltaTime = newDeltaTime; }
	static double LastFrame() { return lastFrame; }
	static void LastFrame(double newLastFrame) { lastFrame = newLastFrame; }
private:
	static double deltaTime, lastFrame;

	
};

