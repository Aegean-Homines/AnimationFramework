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

	
};

