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

// Struct for the spline calculations
// Formula = s = a + b(x-xControl) + c(x-xcontrol)^2 + d(x-xcontrol)^3
struct SplineSet {
	double a;
	double b;
	double c;
	double d;
	double x;
};

class SplineManager
{
public:
	static SplineManager* Instance();
	~SplineManager();

	// Magic numbers as spline nodes
	void BuildSpline();

	// Draw the nodes and lines between them
	void DrawSpline(ShaderProgram program);
	void InsertNode(SplineNode* node);

	// Two functions to move the character
	void RelocateRootNode(SkeletonNode* rootNode, float u);

	// The movement, uses time to find the next point on the spline and the speed
	// returns velocity. It shouldn't because its REALLY not OOP but whatevs.
	float AdvanceOnSpline(SkeletonNode* rootNode, double deltaTime, int totalTime);

	unsigned int GetPointListSize() { return totalList.size(); }
	float GetLastPoint() { return nodeList.back()->nodeLocation.x; }
	float GetFirstPoint() { return nodeList.front()->nodeLocation.x; }

	// Gets the physical translate value using the u value
	glm::vec3 GetPointAtParametricValue(float u);
	glm::vec3 GetPointAtX(float x, SplineSet const & splineSet);
private:
	SplineNodeList nodeList;
	SplineNodeList insertedNodeList;
	SplineNodeList totalList;
	float velocityTimer = 0.0f;

	std::vector<SplineSet> splineSetVector;

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

