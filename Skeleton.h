#pragma once

#include <vector>

using std::vector;

class SkeletonNode;

class Skeleton
{
public:
	Skeleton();
	~Skeleton();

	void RegisterJoint(SkeletonNode* newNode);
	SkeletonNode* GetJoint(unsigned int index);
	SkeletonNode* GetRoot();
	void CalculateAllTransformsUsingIK();
	void ResetAllNodes();
private:
	vector<SkeletonNode*> skeleton;

};

