#pragma once

#include <glm.hpp>
#include <vector>
#include <unordered_map>
#include "Types.h"
#include "ShaderProgram.h"

using glm::vec3;
using glm::mat4;
using std::vector;

class SkeletonNode
{
public:
	SkeletonNode(std::string const & nodeName = "");
	~SkeletonNode();

	// Methods
	void Draw(ShaderProgram const & program);
	SkeletonNode* AddSkeletonNode(vec3 translate, vec3 rotate, vec3 scale, MeshType meshType, std::string const & nodeName = "");
	void AddSkeletonNode(SkeletonNode* child);
	SkeletonNode* GetChild(unsigned int index) { if (index <= children.size()) return children[index]; return NULL; }
	// Getter & Setter
	MeshType GetMeshType() const { return meshType; }
	void SetMeshType(MeshType newMeshType) { meshType = newMeshType; }
	SkeletonNode* Parent() const { return parent; }
	void Parent(SkeletonNode* newParent) { parent = newParent; }

	// Not object oriented and I'm not happy with this as well but it clutters up the entire code when I had all getters and setters
	// So this is a reminder to myself: this is a one-shot framework for a small project
	// Code doesn't have to be beautiful and elegant - it needs to be written fast
	// Deal with it.
	vec3 localTranslate, localRotate, localScale;
	mat4 worldTransformation;
	int level;
private:
	// DATA
	MeshType meshType;
	vector<SkeletonNode*> children;
	SkeletonNode* parent = NULL;
	std::string nodeName;

	// HELPER METHODS
	void ResetTransformMatrix();
	void DrawLinesBetweenNodes(ShaderProgram const & program);

};

