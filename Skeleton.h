#pragma once

#include <vector>
#include <glm.hpp>

#include "Types.h"
#include "ShaderProgram.h"

using std::vector;
using glm::vec3;

class SkeletonNode;

class Skeleton
{
public:
	Skeleton();
	~Skeleton();

	void AddSkeletonNode(vec3 translate, vec3 rotate, vec3 scale, MeshType meshType);
	void Draw(ShaderProgram const & program);
private:
	vector<SkeletonNode*> skeletonNodes;

};

