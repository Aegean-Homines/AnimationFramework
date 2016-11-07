#include "SplineManager.h"
#include "GraphicsManager.h"
#include "SkeletonNode.h"
#include "Mesh.h"
#include "Types.h"

#include <gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>

#define INTERVAL 10

SplineManager::SplineManager()
{
	currentSplineNodeIndex = 0;
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
	std::cout << "Original Points" << std::endl;
	PrintVector(nodeList);
	Spline();
	for (unsigned int i = 0; i < splineSetVector.size(); ++i)
		std::cout << splineSetVector[i].d << "\t" << splineSetVector[i].c << "\t" << splineSetVector[i].b << "\t" << splineSetVector[i].a << "\t" << splineSetVector[i].x << std::endl;

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

void SplineManager::RelocateRootNode(SkeletonNode * rootNode)
{
	if (totalList.size() != 0) {
		rootNode->transformVQS.translate = totalList[currentSplineNodeIndex]->nodeLocation;
	}
}

void SplineManager::CurrentSplineNodeIndex(unsigned int index)
{
	currentSplineNodeIndex = index % totalList.size();
}

vec3 const & SplineManager::NextSplineNode()
{
	if (currentSplineNodeIndex + 1 == totalList.size())
		return totalList[0]->nodeLocation;

	return totalList[currentSplineNodeIndex + 1]->nodeLocation;
}

void SplineManager::AdvanceOnSpline(SkeletonNode* rootNode, double deltaTime)
{
	currentSplineNodeIndex = (int)(floor(deltaTime)) % (totalList.size() - 1);
	// rotate the model towards the next node and move it
	Quaternion & rootRotate = rootNode->transformVQS.rotate;
	RelocateRootNode(rootNode);
	rootNode->LookAt(totalList[currentSplineNodeIndex + 1]->nodeLocation);
}

float SplineManager::GetPointAtParametricValue(float u)
{
	/*float minPoint = nodeList[0]->nodeLocation.x;
	float maxPoint = nodeList[nodeList.size() - 1]->nodeLocation.x;
	
	float interval = maxPoint - minPoint;
	float realXEquivalent = (u * interval) + minPoint;

	for (unsigned int i = 0; i < splineSetVector.size(); ++i) {
		// find the function interval for that point
		if(realXEquivalent >= splineSetVector[i].x)
	}
	*/

	return 0.0f;
}

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
			// eq parts
			double a = currentSet.a;
			double b = currentSet.b * (pointX - x);
			double c = currentSet.c * pow(pointX - x, 2);
			double d = currentSet.d * pow(pointX - x, 3);
			// final result
			float y = static_cast<float>(a + b + c + d);		
			totalList.push_back(new SplineNode(vec3(pointX, 0, y)));
			insertedNodeList.push_back(new SplineNode(vec3(pointX, 0, y)));
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
