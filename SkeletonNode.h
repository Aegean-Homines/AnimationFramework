#pragma once

#include <glm.hpp>
#include <vector>
#include <map>
#include "Types.h"
#include "ShaderProgram.h"
#include "VQS.h"

using glm::vec3;
using glm::mat4;
using std::vector;
using std::map;

class SkeletonNode
{
public:
	typedef map<int, VQS> AnimationTransformationMap;

	SkeletonNode(std::string const & nodeName = "", vec3 const & color = vec3(1.0f));
	~SkeletonNode();
	
	// Methods
	void Draw(ShaderProgram const & program, float elapseTime);
	SkeletonNode* AddSkeletonNode(MeshType meshType, std::string const & nodeName = "");
	void AddSkeletonNode(SkeletonNode* child);
	SkeletonNode* GetChild(unsigned int index) { if (index < children.size()) return children[index]; return NULL; }
	void ToWorldSpace();
	void MoveAllToWorldSpace();

	// Getter & Setter
	MeshType GetMeshType() const { return meshType; }
	void SetMeshType(MeshType newMeshType) { meshType = newMeshType; }
	SkeletonNode* Parent() const { return parent; }
	void Parent(SkeletonNode* newParent) { parent = newParent; }
	std::string Name() { return nodeName; }
	void Name(std::string name) { nodeName = name; }
	void SetVQSAtIndex(int index, VQS const & vqs);
	VQS & GetVQSAtIndex(int index);

	void Insert(int keyFrameTime, VQS transformation);
	void ScaleSkeleton(float scalingFactor);
	void CalculateTransformVQS(float time);
	void CalculateAllTransforms(float time);

	// Visual fun stuff
	void ColorSkeletonUniformly(vec3 const & color);
	void ColorSkeletonByLevel(vec3 const & color);
	void DrawLineToPoint(ShaderProgram const & program, vec3 const & point, vec3 const & color = vec3(1.0f));
	bool LookAt(vec3 const & targetLocation);

	VQS globalVQS;
	VQS localVQS;
	VQS initialVQS; //For reseting the skeleton
	int level;
private:
	// DATA
	MeshType meshType;
	vector<SkeletonNode*> children;
	SkeletonNode* parent = NULL;
	std::string nodeName;
	AnimationTransformationMap transformationMap;
	vec3 nodeColor;
	vec3 forwardVector;
	vec3 upVector;
	// HELPER METHODS
	void DrawLinesBetweenNodes(ShaderProgram const & program);

};

