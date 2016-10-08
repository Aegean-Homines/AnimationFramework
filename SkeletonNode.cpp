#define GLM_FORCE_RADIANS

#include "SkeletonNode.h"
#include "GraphicsManager.h"

#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#define PI 3.14159265359f
const float angleMultiplication = (PI / 180.0f);

SkeletonNode::SkeletonNode()
{
}


SkeletonNode::~SkeletonNode()
{
}

void SkeletonNode::Draw(ShaderProgram const & program)
{
	if (parent != NULL) {
		Mesh* myMesh = GraphicsManager::GetMesh(meshType);

		glm::mat4 transform;
		transform = glm::translate(transform, Translate);
		transform = glm::rotate(transform, Rotate.x/* * angleMultiplication*/, vec3(1.0f, 0.0f, 0.0f));
		transform = glm::rotate(transform, Rotate.y/* * angleMultiplication*/, vec3(0.0f, 1.0f, 0.0f));
		transform = glm::rotate(transform, Rotate.z/* * angleMultiplication*/, vec3(0.0f, 0.0f, 1.0f));
		transform = glm::scale(transform, Scale);

		GLint transformLocation = glGetUniformLocation(program.program, "Transform");
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &(transform[0][0]));

		myMesh->Draw(program);
	}

	for (unsigned int i = 0; i < children.size(); ++i) {
		children[i]->Draw(program);
	}
}

void SkeletonNode::AddSkeletonNode(vec3 translate, vec3 rotate, vec3 scale, MeshType meshType)
{
	SkeletonNode* node = new SkeletonNode();
	node->Translate = translate;
	node->Rotate = rotate;
	node->Scale = scale;
	node->SetMeshType(meshType);
	node->parent = this;

	children.push_back(node);
}
