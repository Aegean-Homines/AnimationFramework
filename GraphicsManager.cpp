#include "GraphicsManager.h"
#include "WindowManager.h"
#include "EventManager.h"
#include "SkeletonNode.h"

#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <iostream>

using glm::vec3;
using std::vector;

std::string shaderLocation = "Shaders/";
std::string vertexShaderSuffix = ".vert";
std::string fragmentShaderSuffix = ".frag";

std::string denemeShaderName = "Deneme";

ShaderProgram GraphicsManager::denemeShader;
VAO GraphicsManager::vao;
bool GraphicsManager::isWireframeModeOn = false;
int GraphicsManager::width;
int GraphicsManager::height;
MeshMap GraphicsManager::meshMap;
Camera GraphicsManager::camera;

GraphicsManager::GraphicsManager()
{
}


GraphicsManager::~GraphicsManager()
{
}

void GraphicsManager::Render()
{
	glClearColor(0.2f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	denemeShader.Use();

	glm::mat4 ViewMatrix;
	ViewMatrix = camera.GetViewMatrix();
	//ViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, -10.0f));

	glm::mat4 ProjectionMatrix;
	ProjectionMatrix = glm::perspective(glm::radians(90.0f), (float)width / (float)height, 0.1f, 1000.0f);

	int location;
	location = glGetUniformLocation(denemeShader.program, "ViewMatrix");
	glUniformMatrix4fv(location, 1, GL_FALSE, &(ViewMatrix[0][0]));
	location = glGetUniformLocation(denemeShader.program, "ProjectionMatrix");
	glUniformMatrix4fv(location, 1, GL_FALSE, &(ProjectionMatrix[0][0]));

	skeleton.Draw(denemeShader);

	denemeShader.Unuse();

	glfwSwapBuffers(WindowManager::GetWindow());
}

void GraphicsManager::Update()
{
	// Toggle debug mode
	if (EventManager::IsKeyPressed(GLFW_KEY_N))
		isWireframeModeOn = !isWireframeModeOn;

	camera.Update();
}

void GraphicsManager::ResetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	glViewport(x, y, width, height);
	GraphicsManager::width = width;
	GraphicsManager::height = height;
}

void GraphicsManager::InitializeGraphics()
{
	int width, height;
	glfwGetFramebufferSize(WindowManager::GetWindow(), &width, &height);

	ResetViewport(0, 0, width, height);

	glEnable(GL_DEPTH_TEST);
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
	/*vector<Vertex> vertices;
	vertices.push_back(Vertex(vec3(-1.0f, -1.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f)));
	vertices.push_back(Vertex(vec3(1.0f, -1.0f, 0.0f), vec3(0.0f, 1.0f, 1.0f)));
	vertices.push_back(Vertex(vec3(1.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)));
	vertices.push_back(Vertex(vec3(-1.0f, 1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)));

	vector<GLuint> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);*/

	// CUBE
	vector<Vertex> vertices;
	// Bottom left
	vertices.push_back(Vertex(vec3(-1.0f, -1.0f, -1.0f), vec3(1.0f, 0.0f, 0.0f))); // front 0
	vertices.push_back(Vertex(vec3(-1.0f, -1.0f, 1.0f), vec3(0.0f, 1.0f, 1.0f))); // back 1

	// Bottom right
	vertices.push_back(Vertex(vec3(1.0f, -1.0f, -1.0f), vec3(0.0f, 0.0f, 1.0f))); // front 2
	vertices.push_back(Vertex(vec3(1.0f, -1.0f, 1.0f), vec3(1.0f, 0.0f, 1.0f))); // back 3

	// Top right
	vertices.push_back(Vertex(vec3(1.0f, 1.0f, -1.0f), vec3(0.0f, 0.0f, 1.0f))); // front 4
	vertices.push_back(Vertex(vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 1.0f)));	// back 5

	// Top left
	vertices.push_back(Vertex(vec3(-1.0f, 1.0f, -1.0f), vec3(1.0f, 1.0f, 0.0f))); // front 6
	vertices.push_back(Vertex(vec3(-1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f))); // back 7

	vector<GLuint> indices;
	// Front face
	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(4);
	indices.push_back(0);
	indices.push_back(4);
	indices.push_back(6);

	// Back face
	indices.push_back(1);
	indices.push_back(3);
	indices.push_back(5);
	indices.push_back(1);
	indices.push_back(5);
	indices.push_back(7);

	// Left face
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(7);
	indices.push_back(0);
	indices.push_back(7);
	indices.push_back(6);

	// Right face
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(5);
	indices.push_back(2);
	indices.push_back(5);
	indices.push_back(4);

	// Top face
	indices.push_back(6);
	indices.push_back(4);
	indices.push_back(5);
	indices.push_back(6);
	indices.push_back(5);
	indices.push_back(7);

	// Bottom face
	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(1);

	Mesh* cubeMesh = new Mesh(vertices, indices);
	meshMap.insert(std::pair<MeshType, Mesh*>(CUBE, cubeMesh));

	// Create skeleton
	skeleton.AddSkeletonNode(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), CUBE);
	skeleton.AddSkeletonNode(vec3(8.0f, 2.0f, -10.0f), vec3(-45.0f, 45.0f, 45.0f), vec3(2.0f, 2.0f, 2.0f), CUBE);

	SkeletonNode* skeletonNode = skeleton.GetChild(0);
	skeletonNode->AddSkeletonNode(vec3(0.0f, -3.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.4f, 1.4f, 1.4f), CUBE);

	// Set Camera
	camera.CameraPosition(vec3(0.0f, 0.0f, 5.0f));
	camera.CameraTarget(vec3(0.0f, 0.0f, 0.0f));
}

void GraphicsManager::SetWireframeMode(bool wireframeMode)
{
	isWireframeModeOn = wireframeMode;

	if (isWireframeModeOn)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

Mesh* GraphicsManager::GetMesh(MeshType type)
{
	return meshMap.at(type);
}
