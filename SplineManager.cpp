#include "SplineManager.h"
#include "GraphicsManager.h"
#include "Mesh.h"
#include "Types.h"

#include <gtc/matrix_transform.hpp>

SplineManager::SplineManager()
{
}


SplineManager* SplineManager::instance;

SplineManager* SplineManager::Instance()
{
	if (!instance)
		instance = new SplineManager();
	return instance;
}

SplineManager::~SplineManager()
{
	SplineNodeListIterator iter = nodeList.begin();
	while (iter != nodeList.end()) {
		delete *(iter++);
	}
}

void SplineManager::DrawSpline(ShaderProgram program)
{
	Mesh* myMesh = GraphicsManager::GetMesh(CUBE);

	vec3 nodeColor(1.0f, 0.0f, 1.0f);
	GLint colorLocation = glGetUniformLocation(program.program, "Color");
	glUniform3fv(colorLocation, 1, &nodeColor[0]);

	SplineNodeListIterator iter = nodeList.begin();
	while (iter != nodeList.end()) {
		mat4 worldTransformation(1.0f);
		worldTransformation = glm::translate(worldTransformation, (*iter)->nodeLocation);

		GLint transformLocation = glGetUniformLocation(program.program, "Transform");
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &(worldTransformation[0][0]));

		myMesh->Draw(program);
		++iter;
	}

	DrawConnectionBetweenNodes(program);
}

void SplineManager::InsertNode(SplineNode* node)
{
	nodeList.push_back(node);
}

void SplineManager::DrawConnectionBetweenNodes(ShaderProgram program)
{
	Mesh* myMesh = GraphicsManager::GetMesh(LINE);

	// We're sending identity matrix as its model
	mat4 identityMatrix;
	GLint transformLocation = glGetUniformLocation(program.program, "Transform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &(identityMatrix[0][0]));

	vec3 nodeColor(1.0f, 1.0f, 1.0f);
	GLint colorLocation = glGetUniformLocation(program.program, "Color");
	glUniform3fv(colorLocation, 1, &nodeColor[0]);

	std::vector<Vertex> newVertices;
	newVertices.resize(2);
	SplineNodeListIterator secondElement = nodeList.begin();
	SplineNodeListIterator firstElement = secondElement++;
	while (secondElement != nodeList.end()) {
		mat4 parentTransform = glm::translate(mat4(1.0f), (*firstElement)->nodeLocation);
		mat4 worldTransformation = glm::translate(mat4(1.0f), (*secondElement)->nodeLocation);

		vec3 parentTranslate = vec3(parentTransform[3][0], parentTransform[3][1], parentTransform[3][2]);
		vec3 myTranslate = vec3(worldTransformation[3][0], worldTransformation[3][1], worldTransformation[3][2]);
		newVertices[0] = (Vertex(parentTranslate, vec3(1.0f)));
		newVertices[1] = (Vertex(myTranslate, vec3(1.0f)));

		myMesh->BufferNewData(newVertices);
		myMesh->Draw(program);

		firstElement++;
		secondElement++;
	}
	
}
