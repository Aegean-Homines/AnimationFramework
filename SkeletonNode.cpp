#define GLM_FORCE_RADIANS

#include "SkeletonNode.h"
#include "GraphicsManager.h"
#include "EventManager.h"
#include "Quaternion.h"

#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/quaternion.hpp>

#define PI 3.14159265359f
#define SCALING_FACTOR 1.0f

const float angleMultiplication = (PI / 180.0f);

int SkeletonNode::frame = 0;

int SkeletonNode::maxFrame;

SkeletonNode::SkeletonNode(std::string const & nodeName): nodeName(nodeName)
{
	level = 0;
	meshType = CUBE;
	currentFrameTime = 0;
}


SkeletonNode::~SkeletonNode()
{
}

void SkeletonNode::Draw(ShaderProgram const & program)
{
	bool j, k, l;
	j = EventManager::IsKeyPressed(GLFW_KEY_J);
	k = EventManager::IsKeyPressed(GLFW_KEY_K);
	l = EventManager::IsKeyPressed(GLFW_KEY_L);

	if (parent != NULL) {
		Mesh* myMesh = GraphicsManager::GetMesh(meshType);

		FbxAMatrix fbxTransform = transformationMap[frame];
		// Reset matrix to identity
		worldTransformation = mat4(1.0f);
		
		FbxDouble3 fbxTranslate, fbxRotate;
		fbxTranslate = fbxTransform.GetT();
		fbxRotate = fbxTransform.GetR() * angleMultiplication;
		// Translate, rotate and scale
		GLfloat rotationX, rotationY, rotationZ;
		GLfloat translateX, translateY, translateZ;
		rotationX = static_cast<float>(fbxRotate.mData[0]);
		rotationY = static_cast<float>(fbxRotate.mData[1]);
		rotationZ = static_cast<float>(fbxRotate.mData[2]);
		translateX = static_cast<float>(fbxTranslate.mData[0]);
		translateY = static_cast<float>(fbxTranslate.mData[1]);
		translateZ = static_cast<float>(fbxTranslate.mData[2]);

		vec3 localTranslate(translateX, translateY, translateZ);
		vec3 localRotate(rotationX, rotationY, rotationZ);

		worldTransformation = glm::translate(worldTransformation, localTranslate);
		Quaternion rotationQuat(localRotate);
		rotationQuat.Normalize();
		mat4 rotationMatrix = rotationQuat.RotationMatrix();
		worldTransformation = worldTransformation * rotationMatrix;
		worldTransformation = glm::scale(worldTransformation, vec3(1.0f));

		/*FbxAMatrix fbxTransform = transformationMap[frame];
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				worldTransformation[i][j] = fbxTransform[i][j];*/

		//worldTransformation = transformationMap[frame];
		// Get parent transform matrix and concat with my current one to find its place in world coords
		mat4 & parentTransform = parent->worldTransformation;
		worldTransformation = parentTransform * worldTransformation;

		GLint transformLocation = glGetUniformLocation(program.program, "Transform");
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &(worldTransformation[0][0]));

		myMesh->Draw(program);

		// Draw lines between points
		DrawLinesBetweenNodes(program);
	}
	else {
		frame = frame++ % maxFrame;
	}

	for (unsigned int i = 0; i < children.size(); ++i) {
		children[i]->Draw(program);
	}

	
}

void SkeletonNode::AddSkeletonNode(SkeletonNode* child)
{
	children.push_back(child);
}

SkeletonNode* SkeletonNode::AddSkeletonNode(MeshType meshType, std::string const & nodeName /*= ""*/)
{
	SkeletonNode* node = new SkeletonNode();
	/*node->localTranslate = translate;
	node->localRotate = rotate * angleMultiplication; // Convert to radian
	node->localScale = scale * SCALING_FACTOR;*/
	node->SetMeshType(meshType);
	node->parent = this;
	node->nodeName = nodeName;

	children.push_back(node);

	return node;
}

void SkeletonNode::Insert(int keyFrameTime, FbxAMatrix transformation)
{
	transformationMap[keyFrameTime] = transformation;
}

void SkeletonNode::ResetTransformMatrix()
{
	memset(&worldTransformation[0][0], 0, 16 * sizeof(float));
	worldTransformation[0][0] = worldTransformation[1][1] = worldTransformation[2][2]= worldTransformation[3][3] = 1.0f;
}

void SkeletonNode::DrawLinesBetweenNodes(ShaderProgram const & program)
{

	// for some weird thing between root and its children
	// remove this after figuring out a cleaner way of handling it
	if (parent == NULL || parent->parent == NULL)
		return;

	Mesh* myMesh = GraphicsManager::GetMesh(LINE);

	// We're sending identity matrix as its model
	mat4 identityMatrix;
	GLint transformLocation = glGetUniformLocation(program.program, "Transform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &(identityMatrix[0][0]));

	// Get parent's transform value
	mat4 & parentTransform = parent->worldTransformation;

	// We'll buffer position values of this node and it's parent to draw a line between them
	std::vector<Vertex> newVertices;
	vec3 parentTranslate = vec3(parentTransform[3][0], parentTransform[3][1], parentTransform[3][2]);
	vec3 myTranslate = vec3(worldTransformation[3][0], worldTransformation[3][1], worldTransformation[3][2]);
	newVertices.push_back(Vertex(parentTranslate, vec3(1.0f)));
	newVertices.push_back(Vertex(myTranslate, vec3(1.0f)));

	// Finally buffer new vertices and draw the lines
	myMesh->BufferNewData(newVertices);
	myMesh->Draw(program);
}
