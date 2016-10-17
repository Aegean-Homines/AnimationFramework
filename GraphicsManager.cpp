#include "GraphicsManager.h"
#include "WindowManager.h"
#include "EventManager.h"
#include "SkeletonNode.h"
#include "ModelManager.h"

#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <iostream>

#define CUBE_SCALE 0.03f
#define PI 3.14159265359f
const float angleMultiplication = (PI / 180.0f);
using glm::vec3;
using std::vector;

std::string shaderLocation = "Shaders/";
std::string vertexShaderSuffix = ".vert";
std::string fragmentShaderSuffix = ".frag";

std::string simpleShaderName = "Simple";

ShaderProgram GraphicsManager::simpleShader;
VAO GraphicsManager::vao;
bool GraphicsManager::isWireframeModeOn = false;
int GraphicsManager::width;
int GraphicsManager::height;
MeshMap GraphicsManager::meshMap;
Camera GraphicsManager::camera;

SkeletonNode GraphicsManager::node;

void GraphicsManager::DrawGround(ShaderProgram& program)
{
	Mesh* myMesh = GraphicsManager::GetMesh(CUBE);

	// Reset matrix to identity
	glm::mat4 worldTransformation = mat4(1.0f);
	// Translate, rotate and scale

	vec3 translate = vec3(0.0f, -1.0f, 0.0f);
	vec3 rotate = vec3(90.0f, 0.0f, 0.0f);
	vec3 scale = vec3(20.0f, 20.0f, 1.0f);

	worldTransformation = glm::translate(worldTransformation, translate);
	worldTransformation = glm::rotate(worldTransformation, rotate.z, vec3(0.0f, 0.0f, 1.0f));
	worldTransformation = glm::rotate(worldTransformation, rotate.y, vec3(0.0f, 1.0f, 0.0f));
	worldTransformation = glm::rotate(worldTransformation, rotate.x, vec3(1.0f, 0.0f, 0.0f));
	worldTransformation = glm::scale(worldTransformation, scale);

	GLint transformLocation = glGetUniformLocation(program.program, "Transform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &(worldTransformation[0][0]));

	myMesh->Draw(program);
}

GraphicsManager::GraphicsManager()
{
}


GraphicsManager::~GraphicsManager()
{
}

void GraphicsManager::Render()
{
	if (isWireframeModeOn)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	simpleShader.Use();

	glm::mat4 ViewMatrix;
	ViewMatrix = camera.GetViewMatrix();

	glm::mat4 ProjectionMatrix;
	ProjectionMatrix = glm::perspective(glm::radians(90.0f), (float)width / (float)height, 0.1f, 1000.0f);

	int location;
	location = glGetUniformLocation(simpleShader.program, "ViewMatrix");
	glUniformMatrix4fv(location, 1, GL_FALSE, &(ViewMatrix[0][0]));
	location = glGetUniformLocation(simpleShader.program, "ProjectionMatrix");
	glUniformMatrix4fv(location, 1, GL_FALSE, &(ProjectionMatrix[0][0]));

	ModelManager* modelManager = ModelManager::Instance();
	SkeletonNode* rootNode = modelManager->RootNode();
	rootNode->Draw(simpleShader);

	//DrawGround(simpleShader);
	//node.Draw(simpleShader);
	simpleShader.Unuse();
	glfwSwapBuffers(WindowManager::GetWindow());
}

void GraphicsManager::Update()
{
	// Toggle debug mode
	if (EventManager::IsKeyPressed(GLFW_KEY_N)) {
		isWireframeModeOn = !isWireframeModeOn;
	}

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
}

void GraphicsManager::InitializeShaders()
{
	std::string shaderName = shaderLocation + simpleShaderName + vertexShaderSuffix;
	simpleShader.CreateProgram();
	simpleShader.CreateShader(shaderName.c_str(), GL_VERTEX_SHADER);
	shaderName = shaderLocation + simpleShaderName + fragmentShaderSuffix;
	simpleShader.CreateShader(shaderName.c_str(), GL_FRAGMENT_SHADER);
	simpleShader.LinkProgram();
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
	vertices.push_back(Vertex(vec3(-1.0f, -1.0f, -1.0f) * CUBE_SCALE, vec3(1.0f, 1.0f, 1.0f))); // front 0
	vertices.push_back(Vertex(vec3(-1.0f, -1.0f, 1.0f) * CUBE_SCALE, vec3(1.0f, 1.0f, 1.0f))); // back 1

	// Bottom right
	vertices.push_back(Vertex(vec3(1.0f, -1.0f, -1.0f) * CUBE_SCALE, vec3(1.0f, 1.0f, 1.0f))); // front 2
	vertices.push_back(Vertex(vec3(1.0f, -1.0f, 1.0f) * CUBE_SCALE, vec3(1.0f, 1.0f, 1.0f))); // back 3

	// Top right
	vertices.push_back(Vertex(vec3(1.0f, 1.0f, -1.0f) * CUBE_SCALE, vec3(1.0f, 1.0f, 1.0f))); // front 4
	vertices.push_back(Vertex(vec3(1.0f, 1.0f, 1.0f) * CUBE_SCALE, vec3(1.0f, 1.0f, 1.0f)));	// back 5

	// Top left
	vertices.push_back(Vertex(vec3(-1.0f, 1.0f, -1.0f) * CUBE_SCALE, vec3(1.0f, 1.0f, 1.0f))); // front 6
	vertices.push_back(Vertex(vec3(-1.0f, 1.0f, 1.0f) * CUBE_SCALE, vec3(1.0f, 1.0f, 1.0f))); // back 7

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

	Mesh* cubeMesh = new Mesh(vertices, indices, GL_TRIANGLES);
	meshMap.insert(std::pair<MeshType, Mesh*>(CUBE, cubeMesh));

	// Line
	vertices.resize(0);
	indices.resize(0);

	vertices.push_back(Vertex(vec3(1.0f), vec3(1.0f)));
	vertices.push_back(Vertex(vec3(0.0f), vec3(0.0f)));

	indices.push_back(0);
	indices.push_back(1);

	Mesh* lineMesh = new Mesh(vertices, indices, GL_LINES);
	meshMap.insert(std::pair<MeshType, Mesh*>(LINE, lineMesh));

	node.localTranslate = vec3(0.0f, -10.0f, 0.0f);
	node.localScale = vec3(1.0f, 20.0f, 1.0f);
	node.localRotate = vec3(90.0f, 0.0f, 0.0f);
	node.SetMeshType(CUBE);
	/*SkeletonNode* visibleroot = node.AddSkeletonNode(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 90.0f), vec3(1.0f), CUBE, "Child1");
	visibleroot->level = 0;
	SkeletonNode* visible2 = visibleroot->AddSkeletonNode(vec3(0.0f, -6.0f, 0.0f), vec3(0.0f, 0.0f, 90.0f), vec3(1.0f), CUBE, "Child2");
	visible2->level = 1;
	visibleroot->AddSkeletonNode(vec3(3.0f, -6.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f), CUBE, "Child3")->level = 1;
	visible2->AddSkeletonNode(vec3(0.0f, -9.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f), CUBE, "Child4")->level = 2;*/

	// Set Camera
	camera.CameraPosition(vec3(-1.0f, 3.0f, 5.0f));
	camera.CameraTarget(vec3(0.0f, 0.0f, 0.0f));
}

void GraphicsManager::SetWireframeMode(bool wireframeMode)
{
	isWireframeModeOn = wireframeMode;
}

Mesh* GraphicsManager::GetMesh(MeshType type)
{
	return meshMap.at(type);
}
