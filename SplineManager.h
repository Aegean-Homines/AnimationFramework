#pragma once

#include "ShaderProgram.h"

#include <glm.hpp>
#include <vector>

#define MAX_POINT_AMOUNT 20
#define MATRIX_SIZE MAX_POINT_AMOUNT+2
#define INTERVAL 10
#define TOTAL_ANIMATION_IN_SECONDS 1000

using glm::vec3;
using std::vector;

class SkeletonNode;

struct SplineNode {
	vec3 nodeLocation;
	SplineNode() : nodeLocation(0.0f) {};
	SplineNode(vec3 const & location): nodeLocation(location){}
	SplineNode(float x, float y, float z): nodeLocation(vec3(x,y,z)){}
};

typedef std::vector<SplineNode*> SplineNodeList;
typedef std::vector<SplineNode*>::iterator SplineNodeListIterator;
typedef std::vector<SplineNode*>::const_iterator SplineNodeListConstIterator;
typedef std::vector<std::vector<float>> Matrix;

class SplineElement {
public:
	//(t-c)^d
	float coefficient;
	int c;
	int d;
	bool isTruncatedPowerFunction;

	SplineElement(int cVal, int power, bool isTruncated):
		coefficient(1.0f), c(cVal), d(power), isTruncatedPowerFunction(isTruncated)
	{}

	float GetTValue(float tVal)
	{
		float floatD = static_cast<float>(d);

		if (isTruncatedPowerFunction)
		{
			float realT = tVal - c;
			if (realT > 0)
				return (powf(realT, floatD) * coefficient); //Truncated part
			else
				return 0.0f; //regular 0
		}

		//Normal calculation
		return powf(tVal, floatD) * coefficient;
	}
};

typedef std::vector<SplineElement> SplineElementList;

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
	float AdvanceOnSpline(SkeletonNode* rootNode, double deltaTime, int totalTime, bool continuousAnimation = true);

	unsigned int GetPointListSize() { return totalList.size(); }
	float GetLastPoint() { return nodeList.back()->nodeLocation.x; }
	float GetFirstPoint() { return nodeList.front()->nodeLocation.x; }
	float alpha;
	float drawingAlpha;
	bool isAnimationFinished = false;
	// Gets the physical translate value using the u value
	glm::vec3 GetPointAtParametricValue(float u);

	void TargetObjectNewMoved(vec3 const & newPosition);
private:
	SplineNodeList nodeList;
	SplineNodeList insertedNodeList;
	SplineNodeList totalList;
	float velocityTimer = 0.0f;
	float steps;

	SplineElementList function;
	SplineElementList firstDerivative;
	SplineElementList secondDerivative;
	Matrix coeffMatrixX;
	Matrix coeffMatrixY;

	void Spline();
	void DrawConnectionBetweenNodes(ShaderProgram program);
	void CalculatePointsBetweenControlPoints();

	// Helper
	void PrintVector(SplineNodeList const & listToPrint);
	void DrawSplinePoints(SplineNodeList const & pointsToDraw, vec3 const & color, vec3 const & scale, ShaderProgram & program);
	void CalculateFunction();
	void Derive(SplineElementList & functionToDerive);
	void CreateMatrix(Matrix & matrix);
	SplineNode* SplineInterpolate(float t);
	void SolveMatrix(Matrix & matrix);
	vec3 FindLastSplineNode();
	// SingletonStuff
	SplineManager();
	SplineManager(SplineManager const &) {};
	SplineManager& operator=(SplineManager const &) {};
	static SplineManager* instance;
};

