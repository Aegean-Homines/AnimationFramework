#pragma once
class ShaderProgram
{
public:
	int program;

	void CreateProgram();
	void CreateShader(const char* fileName, const int type);
	void LinkProgram();
	void Use();
	void Unuse();
};

