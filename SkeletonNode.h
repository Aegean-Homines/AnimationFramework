#pragma once

#include <glm.hpp>
#include <vector>
#include <unordered_map>
#include "Types.h"
#include "ShaderProgram.h"
#include "VQS.h"

using glm::vec3;
using glm::mat4;
using std::vector;
using std::unordered_map;

class SkeletonNode
{
public:
	typedef unordered_map<int, VQS> AnimationTransformationMap;

	SkeletonNode(std::string const & nodeName = "", vec3 const & color = vec3(1.0f));
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

	void Insert(int keyFrameTime, VQS transformation);
	void ScaleSkeleton(float scalingFactor);

	void ColorSkeletonUniformly(vec3 const & color);
	void ColorSkeletonByLevel(vec3 const & color);

	VQS transformVQS;
	int level;
private:
	// DATA
	MeshType meshType;
	vector<SkeletonNode*> children;
	SkeletonNode* parent = NULL;
	std::string nodeName;
	AnimationTransformationMap transformationMap;
	vec3 nodeColor;
	// HELPER METHODS
	void DrawLinesBetweenNodes(ShaderProgram const & program);

};

