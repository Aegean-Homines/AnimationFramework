#pragma once

#include <glm.hpp>
#include <vector>
#include <unordered_map>
#include "Types.h"
#include "ShaderProgram.h"

#include <fbxsdk.h>

using glm::vec3;
using glm::mat4;
using std::vector;
using std::unordered_map;

class SkeletonNode
{
public:
	typedef unordered_map<int, FbxAMatrix> AnimationTransformationMap;

	static int frame;
	SkeletonNode(std::string const & nodeName = "");
	~SkeletonNode();
	
	// Methods
	void Draw(ShaderProgram const & program, int frame, float interpolationAmount);
	SkeletonNode* AddSkeletonNode(MeshType meshType, std::string const & nodeName = "");
	void AddSkeletonNode(SkeletonNode* child);
	SkeletonNode* GetChild(unsigned int index) { if (index <= children.size()) return children[index]; return NULL; }
	// Getter & Setter
	MeshType GetMeshType() const { return meshType; }
	void SetMeshType(MeshType newMeshType) { meshType = newMeshType; }
	SkeletonNode* Parent() const { return parent; }
	void Parent(SkeletonNode* newParent) { parent = newParent; }

	void Insert(int keyFrameTime, FbxAMatrix transformation);

	// Not object oriented and I'm not happy with this as well but it clutters up the entire code when I had all getters and setters
	// So this is a reminder to myself: this is a one-shot framework for a small project
	// Code doesn't have to be beautiful and elegant - it needs to be written fast
	// Deal with it.
	mat4 worldTransformation;
	int level;
private:
	// DATA
	MeshType meshType;
	vector<SkeletonNode*> children;
	SkeletonNode* parent = NULL;
	std::string nodeName;
	AnimationTransformationMap transformationMap;
	// HELPER METHODS
	void ResetTransformMatrix();
	void DrawLinesBetweenNodes(ShaderProgram const & program);
	void FbxDouble3ToVec3(FbxDouble3 const & fbxVector, vec3 & glmVec);



};

