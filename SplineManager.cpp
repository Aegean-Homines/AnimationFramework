#include "SplineManager.h"
#include "GraphicsManager.h"
#include "SkeletonNode.h"
#include "Mesh.h"
#include "Types.h"
#include "ArcLengthSegmentManager.h"

#include <gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>

#define INTERVAL 10
#define TOTAL_ANIMATION_IN_SECONDS 7000

SplineManager::SplineManager()
{
	//currentSplineNodeIndex = 0;
}

SplineManager* SplineManager::instance;

SplineManager* SplineManager::Instance()
{
	if (!instance)
		instance = new SplineManager();
	return instance;
}

SplineManager::~SplineManager()
{
	SplineNodeListIterator iter = nodeList.begin();
	while (iter != nodeList.end()) {
		delete *(iter++);
	}
}

void SplineManager::BuildSpline()
{
	// Magic numbers for nodes
	InsertNode(new SplineNode(-10.0f, 0.0f, 0.0f));
	InsertNode(new SplineNode(-7.0f, 0.0f, 2.0f));
	InsertNode(new SplineNode(-4.0f, 0.0f, 5.0f));
	InsertNode(new SplineNode(-2.0f, 0.0f, 3.0f));
	InsertNode(new SplineNode(0.0f, 0.0f, 1.0f));
	InsertNode(new SplineNode(3.0f, 0.0f, 2.0f));
	InsertNode(new SplineNode(5.0f, 0.0f, -2.0f));
	InsertNode(new SplineNode(8.0f, 0.0f, 3.0f));
	InsertNode(new SplineNode(12.0f, 0.0f, 0.0f));
	InsertNode(new SplineNode(15.0f, 0.0f, 6.0f));

	//PrintVector(nodeList);
	// Actual spline calculation
	Spline();

	// Divides control points into intervals so that the curve looks smoother
	CalculatePointsBetweenControlPoints();
}

void SplineManager::DrawSpline(ShaderProgram program)
{

	vec3 nodeColor(1.0f, 0.0f, 1.0f);
	vec3 scale(2.0f);
	DrawSplinePoints(nodeList, nodeColor, scale, program);
	nodeColor.z = 0.0f;
	scale = vec3(1.0f);
	DrawSplinePoints(insertedNodeList, nodeColor, scale, program);

	DrawConnectionBetweenNodes(program);
}

void SplineManager::InsertNode(SplineNode* node)
{
	nodeList.push_back(node);
}

void SplineManager::RelocateRootNode(SkeletonNode* rootNode, float u)
{
	// Use u to calculate it's position on the spline
 	glm::vec3 targetPos = GetPointAtParametricValue(u);
	rootNode->transformVQS.translate = targetPos;

}

float SplineManager::AdvanceOnSpline(SkeletonNode* rootNode, double deltaTime, int totalTime)
{
	// Hackish timer resetting, wohoo!
	velocityTimer += deltaTime / TOTAL_ANIMATION_IN_SECONDS;
	if (velocityTimer > 1.0f) {
		velocityTimer = 0.0f;
	}

	ArcLengthSegmentManager* segmentManager = ArcLengthSegmentManager::Instance();
	// calculate s from t
	float arcLength = segmentManager->GetArcLengthUsingTime(velocityTimer);
	// calculate u from s
	float paramVal = segmentManager->FindParametricValue(arcLength);
	// translate node to its next location
	RelocateRootNode(rootNode, paramVal);
	
	// Get the next location using the future time
	float nextTime = velocityTimer + (deltaTime / TOTAL_ANIMATION_IN_SECONDS);
	float nextArcLength = segmentManager->GetArcLengthUsingTime(nextTime);
	paramVal = segmentManager->FindParametricValue(nextArcLength);

	// Get the target of the next location and look at it
	vec3 target = GetPointAtParametricValue(paramVal);
	rootNode->LookAt(target);
	return segmentManager->GetVelocity(velocityTimer);

}

glm::vec3 SplineManager::GetPointAtParametricValue(float u)
{
	float minPoint = nodeList[0]->nodeLocation.x;
	float maxPoint = nodeList[nodeList.size() - 1]->nodeLocation.x;
	
	float interval = maxPoint - minPoint;
	float realXEquivalent = (u * interval) + minPoint;

	for (unsigned int i = 0; i < splineSetVector.size(); ++i) {
		// find the function interval for that point
		// #TODO: to binary search
		if (realXEquivalent >= nodeList[i]->nodeLocation.x && realXEquivalent < nodeList[i+1]->nodeLocation.x) {
			return GetPointAtX(realXEquivalent, splineSetVector[i]);
		}
	}

	return nodeList.back()->nodeLocation;
	
}

glm::vec3 SplineManager::GetPointAtX(float x, SplineSet const & splineSet)
{
	// eq parts
	double a = splineSet.a;
	double b = splineSet.b * (x - splineSet.x);
	double c = splineSet.c * pow(x - splineSet.x, 2);
	double d = splineSet.d * pow(x - splineSet.x, 3);
	// final result
	float y = static_cast<float>(a + b + c + d);

	return vec3(x, 0, y);
}

// #GodBlessWikipedia
void SplineManager::Spline()
{
	splineSetVector.clear();
	std::vector<float> x;
	std::vector<float> y;
	for (unsigned int i = 0; i < nodeList.size(); ++i) {
		x.push_back(nodeList[i]->nodeLocation.x);
		y.push_back(nodeList[i]->nodeLocation.z);
	}

	int n = x.size() - 1;
	std::vector<float> a;
	a.insert(a.begin(), y.begin(), y.end());
	std::vector<float> b(n);
	std::vector<float> d(n);
	std::vector<float> h(n);

	for (int i = 0; i < n; ++i)
		h[i] = x[i + 1] - x[i];

	std::vector<float> alpha(n);
	for (int i = 1; i < n; ++i)
		alpha[i] = ((3 / h[i]) * (a[i + 1] - a[i])) - ((3 / h[i - 1])*(a[i] - a[i - 1]));

	std::vector<float> c(n + 1);
	std::vector<float> l(n + 1);
	std::vector<float> mu(n + 1);
	std::vector<float> z(n + 1);
	l[0] = 1;
	mu[0] = 0;
	z[0] = 0;

	for (int i = 1; i < n; ++i)
	{
		l[i] = (2 * (x[i + 1] - x[i - 1])) - (h[i - 1] * mu[i - 1]);
		mu[i] = h[i] / l[i];
		z[i] = (alpha[i] - (h[i - 1] * z[i - 1])) / l[i];
	}

	l[n] = 1;
	z[n] = 0;
	c[n] = 0;

	for (int j = n - 1; j >= 0; --j)
	{
		c[j] = z[j] - mu[j] * c[j + 1];
		b[j] = ((a[j + 1] - a[j]) / h[j]) - (h[j] * (c[j + 1] + 2 * c[j]) / 3);
		d[j] = (c[j + 1] - c[j]) / (3 * h[j]);
	}

	splineSetVector.resize(n);
	for (int i = 0; i < n; ++i)
	{
		splineSetVector[i].a = a[i];
		splineSetVector[i].b = b[i];
		splineSetVector[i].c = c[i];
		splineSetVector[i].d = d[i];
		splineSetVector[i].x = x[i];
	}
}

void SplineManager::DrawConnectionBetweenNodes(ShaderProgram program)
{
	Mesh* myMesh = GraphicsManager::GetMesh(LINE);

	// We're sending identity matrix as its model
	mat4 identityMatrix;
	GLint transformLocation = glGetUniformLocation(program.program, "Transform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &(identityMatrix[0][0]));

	vec3 nodeColor(1.0f, 1.0f, 1.0f);
	GLint colorLocation = glGetUniformLocation(program.program, "Color");
	glUniform3fv(colorLocation, 1, &nodeColor[0]);

	std::vector<Vertex> newVertices;
	newVertices.resize(2);
	SplineNodeListIterator secondElement = totalList.begin();
	SplineNodeListIterator firstElement = secondElement++;
	while (secondElement != totalList.end()) {
		mat4 parentTransform = glm::translate(mat4(1.0f), (*firstElement)->nodeLocation);
		mat4 worldTransformation = glm::translate(mat4(1.0f), (*secondElement)->nodeLocation);

		vec3 parentTranslate = vec3(parentTransform[3][0], parentTransform[3][1], parentTransform[3][2]);
		vec3 myTranslate = vec3(worldTransformation[3][0], worldTransformation[3][1], worldTransformation[3][2]);
		newVertices[0] = (Vertex(parentTranslate, vec3(1.0f)));
		newVertices[1] = (Vertex(myTranslate, vec3(1.0f)));

		myMesh->BufferNewData(newVertices);
		myMesh->Draw(program);

		firstElement++;
		secondElement++;
	}
	
}

void SplineManager::CalculatePointsBetweenControlPoints()
{
	totalList.clear();
	insertedNodeList.clear();
	for (unsigned int i = 0; i < splineSetVector.size(); ++i) {
		SplineSet currentSet = splineSetVector[i];
		float x = nodeList[i]->nodeLocation.x;
		float xNext = nodeList[i + 1]->nodeLocation.x;
		float xDistancePerInterval = (xNext - x) / INTERVAL;
		totalList.push_back(nodeList[i]);

		// Formula = s = a + b(x-xControl) + c(x-xcontrol)^2 + d(x-xcontrol)^3
		for (int j = 1; j < INTERVAL; ++j) {
			float pointX = x + (xDistancePerInterval * j);
			vec3 point = GetPointAtX(pointX, currentSet);
			totalList.push_back(new SplineNode(point));
			insertedNodeList.push_back(new SplineNode(point));
		}
	}

	totalList.push_back(nodeList[nodeList.size() - 1]);
	PrintVector(totalList);
	
}

void SplineManager::PrintVector(SplineNodeList const & listToPrint)
{
	for (unsigned int i = 0; i < listToPrint.size(); ++i) {
		std::cout << "x: " << listToPrint[i]->nodeLocation.x << " y: " << listToPrint[i]->nodeLocation.y << " z: " << listToPrint[i]->nodeLocation.z << std::endl;
	}
}

void SplineManager::DrawSplinePoints(SplineNodeList const & pointsToDraw, vec3 const & color, vec3 const & scale, ShaderProgram & program)
{
	Mesh* myMesh = GraphicsManager::GetMesh(CUBE);

	GLint colorLocation = glGetUniformLocation(program.program, "Color");
	glUniform3fv(colorLocation, 1, &color[0]);

	// Drawing points
	SplineNodeListConstIterator iter = pointsToDraw.begin();
	while (iter != pointsToDraw.end()) {
		mat4 worldTransformation(1.0f);
		worldTransformation = glm::translate(worldTransformation, (*iter)->nodeLocation);
		worldTransformation = glm::scale(worldTransformation, scale);
		GLint transformLocation = glGetUniformLocation(program.program, "Transform");
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &(worldTransformation[0][0]));

		myMesh->Draw(program);
		++iter;
	}
}
