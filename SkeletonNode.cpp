#define GLM_FORCE_RADIANS

#include "SkeletonNode.h"
#include "GraphicsManager.h"
#include "EventManager.h"
#include "Quaternion.h"
#include <iostream>

#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#define PI 3.14159265359f
#define SCALING_FACTOR 1.0f
#define INTERPOLATION_VALUE 0.5f

const float angleMultiplication = (PI / 180.0f);

SkeletonNode::SkeletonNode(std::string const & nodeName, vec3 const & color): nodeName(nodeName), nodeColor(color)
{
	level = 0;
	meshType = CUBE;
	forwardVector = vec3(0.0f, 0.0f, 1.0f);
}


SkeletonNode::~SkeletonNode()
{
}

void SkeletonNode::Draw(ShaderProgram const & program, int frame, float interpolationAmount)
{
	// I'm using these in case I want to debug something
	/* 
	bool j, k, l;
	j = EventManager::IsKeyPressed(GLFW_KEY_J);
	k = EventManager::IsKeyPressed(GLFW_KEY_K);
	l = EventManager::IsKeyPressed(GLFW_KEY_L);*/

	if (parent != NULL) {
		Mesh* myMesh = GraphicsManager::GetMesh(meshType);
			 
		VQS& currentVQS = transformationMap[frame];
		VQS& nextVQS = transformationMap[frame + 1];

		transformVQS = VQS::Slerp(currentVQS, nextVQS, interpolationAmount);

		// Get parent transform VQS and concat with my current one to find its place in world coords
		VQS& parentTransform = parent->transformVQS;
 		transformVQS = parentTransform * transformVQS;

		// Get matrix data from VQS to find node's final position in world coords
		mat4 worldTransformation(1.0f);
		worldTransformation = glm::translate(worldTransformation, transformVQS.translate);

		// Send matrix to the GPU for rendering purposes
		GLint transformLocation = glGetUniformLocation(program.program, "Transform");
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &(worldTransformation[0][0]));

		GLint colorLocation = glGetUniformLocation(program.program, "Color");
		glUniform3fv(colorLocation, 1, &nodeColor[0]);

		myMesh->Draw(program);

		// Draw lines between points
		DrawLinesBetweenNodes(program);
	}

	// Recursively call draw for all the children of this node
	for (unsigned int i = 0; i < children.size(); ++i) {
		children[i]->Draw(program, frame, interpolationAmount);
	}

	
}

void SkeletonNode::AddSkeletonNode(SkeletonNode* child)
{
	children.push_back(child);
}

SkeletonNode* SkeletonNode::AddSkeletonNode(MeshType meshType, std::string const & nodeName /*= ""*/)
{
	SkeletonNode* node = new SkeletonNode();
	node->SetMeshType(meshType);
	node->parent = this;
	node->nodeName = nodeName;

	children.push_back(node);

	return node;
}

void SkeletonNode::Insert(int keyFrameTime, VQS transformation)
{
	transformationMap[keyFrameTime] = transformation;
}

void SkeletonNode::ScaleSkeleton(float scalingFactor)
{
	transformVQS.scalar = scalingFactor; //sort of cheating
}

void SkeletonNode::ColorSkeletonUniformly(vec3 const & color)
{
	nodeColor = color;
	for (unsigned int i = 0; i < children.size(); ++i) {
		children[i]->ColorSkeletonUniformly(color);
	}
}

void SkeletonNode::ColorSkeletonByLevel(vec3 const & color)
{

}

void SkeletonNode::DrawLinesBetweenNodes(ShaderProgram const & program)
{

	// for some weird thing between root and its children
	// #TODO remove this after figuring out a cleaner way of handling it
	if (parent == NULL)
		return;

	Mesh* myMesh = GraphicsManager::GetMesh(LINE);

	// We're sending identity matrix as its model
	mat4 identityMatrix;
	GLint transformLocation = glGetUniformLocation(program.program, "Transform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &(identityMatrix[0][0]));

	// Get parent's transform value
	mat4 parentTransform = glm::translate(mat4(1.0f), parent->transformVQS.translate);
	mat4 worldTransformation = glm::translate(mat4(1.0f), transformVQS.translate);
	// We'll buffer position values of this node and it's parent to draw a line between them
	// I feel dirty for using graphics hacks to get stuff drawn. I'm pretty sure there is a better way
	// But you know, #YOLO
	std::vector<Vertex> newVertices;
	vec3 parentTranslate = vec3(parentTransform[3][0], parentTransform[3][1], parentTransform[3][2]);
	vec3 myTranslate = vec3(worldTransformation[3][0], worldTransformation[3][1], worldTransformation[3][2]);
	newVertices.push_back(Vertex(parentTranslate, vec3(1.0f)));
	newVertices.push_back(Vertex(myTranslate, vec3(1.0f)));

	// Finally buffer new vertices and draw the lines
	myMesh->BufferNewData(newVertices);
	myMesh->Draw(program);
}
