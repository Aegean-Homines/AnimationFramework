#pragma once

#include "ShaderProgram.h"

#include <glm.hpp>
#include <vector>

using glm::vec3;
using std::vector;

class SkeletonNode;

struct SplineNode {
	vec3 nodeLocation;
	SplineNode(vec3 const & location): nodeLocation(location){}
	SplineNode(float x, float y, float z): nodeLocation(vec3(x,y,z)){}
};

typedef std::vector<SplineNode*> SplineNodeList;
typedef std::vector<SplineNode*>::iterator SplineNodeListIterator;
typedef std::vector<SplineNode*>::const_iterator SplineNodeListConstIterator;


class SplineManager
{
public:
	static SplineManager* Instance();
	~SplineManager();

	void BuildSpline();
	void DrawSpline(ShaderProgram program);
	void InsertNode(SplineNode* node);
	void RelocateRootNode(SkeletonNode* rootNode);
	vec3 const & CurrentSplineNode() { return totalList[currentSplineNodeIndex]->nodeLocation; };
	void CurrentSplineNodeIndex(unsigned int index);
	vec3 const & NextSplineNode();
	void AdvanceOnSpline(SkeletonNode* rootNode, double deltaTime);

	unsigned int GetPointListSize() { return totalList.size(); }
	float GetLastPoint() { return nodeList.back()->nodeLocation.x; }
	float GetFirstPoint() { return nodeList.front()->nodeLocation.x; }
	float GetPointAtParametricValue(float u);
private:
	SplineNodeList nodeList;
	SplineNodeList insertedNodeList;
	SplineNodeList totalList;

	struct SplineSet {
		double a;
		double b;
		double c;
		double d;
		double x;
	};
	std::vector<SplineSet> splineSetVector;

	unsigned int currentSplineNodeIndex;

	void Spline();
	void DrawConnectionBetweenNodes(ShaderProgram program);
	void CalculatePointsBetweenControlPoints();

	// Helper
	void PrintVector(SplineNodeList const & listToPrint);
	void DrawSplinePoints(SplineNodeList const & pointsToDraw, vec3 const & color, vec3 const & scale, ShaderProgram & program);
	// SingletonStuff
	SplineManager();
	SplineManager(SplineManager const &) {};
	SplineManager& operator=(SplineManager const &) {};
	static SplineManager* instance;
};

