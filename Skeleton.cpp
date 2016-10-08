#include "Skeleton.h"
#include "SkeletonNode.h"

Skeleton::Skeleton()
{
}


Skeleton::~Skeleton()
{
	for (unsigned int i = 0; i < skeletonNodes.size(); ++i) {
		delete skeletonNodes[i];
	}
}

void Skeleton::AddSkeletonNode(vec3 translate, vec3 rotate, vec3 scale, MeshType meshType)
{
	/*SkeletonNode* node = new SkeletonNode();
	node->Translate(translate);
	node->Rotate(rotate);
	node->Scale(scale);
	node->SetMeshType(meshType);

	skeletonNodes.push_back(node);*/
}

void Skeleton::Draw(ShaderProgram const & program)
{
	for (unsigned int i = 0; i < skeletonNodes.size(); ++i) {
		skeletonNodes[i]->Draw(program);
	}
}
