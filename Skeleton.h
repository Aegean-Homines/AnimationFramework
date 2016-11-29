#pragma once

#include <vector>

using std::vector;

enum OrderingTechnique {
	REVERSE,
	NONE,
	ORDERING_COUNT
};

class SkeletonNode;

// A wrapper class for all the joints (skeleton nodes)
class Skeleton
{
public:
	Skeleton();
	~Skeleton();

	void RegisterJoint(SkeletonNode* newNode);
	SkeletonNode* GetJoint(unsigned int index);
	SkeletonNode* GetRoot();
	void CalculateAllTransformsUsingIK(); //IK is done in this block
	void ResetAllNodes();
private:
	vector<SkeletonNode*> skeleton;

	void OrderNodes(vector<SkeletonNode*> const & skeleton, vector<SkeletonNode*> & orderedList, OrderingTechnique);

};

