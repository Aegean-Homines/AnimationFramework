#include "GraphicsManager.h"
#include "WindowManager.h"
#include "glew.h"
#include "glfw3.h"

#include <iostream>

std::string shaderLocation = "Shaders/";
std::string vertexShaderSuffix = ".vert";
std::string fragmentShaderSuffix = ".frag";

std::string denemeShaderName = "Deneme";

ShaderProgram GraphicsManager::denemeShader;
VAO GraphicsManager::vao;

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

	denemeShader.Use();
	vao.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 3);
	vao.Unbind();

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

void GraphicsManager::InitializeShaders()
{
	std::string shaderName = shaderLocation + denemeShaderName + vertexShaderSuffix;
	denemeShader.CreateProgram();
	denemeShader.CreateShader(shaderName.c_str(), GL_VERTEX_SHADER);
	shaderName = shaderLocation + denemeShaderName + fragmentShaderSuffix;
	denemeShader.CreateShader(shaderName.c_str(), GL_FRAGMENT_SHADER);
	denemeShader.LinkProgram();
}

void GraphicsManager::InitializeData()
{
	// Triangle
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);

	vao.Generate();
	vao.Bind();

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	vao.Unbind();
}
