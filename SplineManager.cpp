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
	InsertNode(new SplineNode(-3.0f, 0.0f, 0.0f));
	/*InsertNode(new SplineNode(-4.0f, 0.0f, 5.0f));
	InsertNode(new SplineNode(-2.0f, 0.0f, 3.0f));
	InsertNode(new SplineNode(0.0f, 0.0f, 1.0f));
	InsertNode(new SplineNode(3.0f, 0.0f, 2.0f));
	InsertNode(new SplineNode(5.0f, 0.0f, -2.0f));
	InsertNode(new SplineNode(8.0f, 0.0f, 3.0f));
	InsertNode(new SplineNode(12.0f, 0.0f, 0.0f));
	InsertNode(new SplineNode(15.0f, 0.0f, 6.0f));*/


	steps = static_cast<float>((nodeList.size()-1) * INTERVAL);
	drawingAlpha = 1.0f / steps;
	alpha = 0.005f;
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

float SplineManager::AdvanceOnSpline(SkeletonNode* rootNode, double deltaTime, int totalTime, bool continuous)
{
	// Hackish timer resetting, wohoo!
	velocityTimer += static_cast<float>(deltaTime / TOTAL_ANIMATION_IN_SECONDS);
	if (velocityTimer > 1.0f) {
		velocityTimer = continuous ? 0.0f : 1.0f;
		isAnimationFinished = true;
	}
	ArcLengthSegmentManager* segmentManager = ArcLengthSegmentManager::Instance();
	if (!isAnimationFinished) {
		// calculate s from t
		float arcLength = segmentManager->GetArcLengthUsingTime(velocityTimer);
		// calculate u from s
		float paramVal = segmentManager->FindParametricValue(arcLength);
		// translate node to its next location
		RelocateRootNode(rootNode, paramVal);

		// Get the next location using the future time
		float nextTime = static_cast<float>(velocityTimer + (deltaTime / TOTAL_ANIMATION_IN_SECONDS));
		float nextArcLength = segmentManager->GetArcLengthUsingTime(nextTime);
		paramVal = segmentManager->FindParametricValue(nextArcLength);

		// Get the target of the next location and look at it
		vec3 target = GetPointAtParametricValue(paramVal);
		rootNode->LookAt(target);
	}
	
	return segmentManager->GetVelocity(velocityTimer);

}

glm::vec3 SplineManager::GetPointAtParametricValue(float u)
{
	int index = static_cast<int>(u / alpha);
	vec3 prevLoc = totalList[index]->nodeLocation;
	vec3 nextLoc;
	if (index == totalList.size() - 1) {
		nextLoc = prevLoc;
	}
	else {
		nextLoc = totalList[index + 1]->nodeLocation;
	}

	float delta = (u / alpha) - index;
	return (nextLoc * delta + prevLoc * (1 - delta));
}

// #GodBlessWikipedia
void SplineManager::Spline()
{
	function.reserve(MATRIX_SIZE);
	CalculateFunction();

	std::cout << "f(t) initialized" << std::endl;

	firstDerivative = function;
	Derive(firstDerivative);

	std::cout << "First derivative of f(t) initialized" << std::endl;

	secondDerivative = firstDerivative;
	Derive(secondDerivative);

	std::cout << "Second derivative of f(t) initialized" << std::endl;

	coeffMatrixX.resize(MATRIX_SIZE);
	coeffMatrixY.resize(MATRIX_SIZE);
	for (unsigned int i = 0; i < MATRIX_SIZE; ++i) {
		coeffMatrixX[i].resize(MATRIX_SIZE + 1);
		coeffMatrixY[i].resize(MATRIX_SIZE + 1);
	}
	CreateMatrix(coeffMatrixX);
	CreateMatrix(coeffMatrixY);

	std::cout << "Matrix creation is complete." << std::endl;

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

	if (nodeList.size() == 0) {
		nodeList.push_back(new SplineNode());
		nodeList.push_back(new SplineNode());
	}else if (nodeList.size() == 1) {
		nodeList.push_back(new SplineNode());
	}

	// First node
	totalList.push_back(nodeList[0]);

	// Calculate rest

	for (float t = alpha; t < 1.0f; t+=alpha) {
		totalList.push_back(SplineInterpolate(t));
	}

	for (float t = drawingAlpha; t < 1.0f; t += drawingAlpha) {
		insertedNodeList.push_back(SplineInterpolate(t));
	}

	// last node
	totalList.push_back(nodeList[nodeList.size() - 1]);
	//PrintVector(totalList);
	
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

void SplineManager::CalculateFunction()
{
	// a values => cVal = 0, toThePowerOf = j
	unsigned int j = 0;
	for (; j < 4; ++j)
	{
		function.push_back(SplineElement(0, j, false));
	}

	//b values => cVal = j-3, toThePowerOf = 3;
	for (; j < function.capacity(); ++j)
	{
		function.push_back(SplineElement(j - 3, 3, true));
	}
}

void SplineManager::Derive(SplineElementList & functionToDerive)
{
	for (unsigned int i = 0; i < functionToDerive.size(); ++i)
	{
		float coefficient = functionToDerive[i].coefficient;
		int toThePowerOf = functionToDerive[i].d;

		coefficient *= toThePowerOf--;

		functionToDerive[i].coefficient = coefficient;
		functionToDerive[i].d = toThePowerOf < 0 ? 0 : toThePowerOf;
	}
}

void SplineManager::CreateMatrix(Matrix & matrix)
{
	int i = 0;

	//creating f(t) part
	for (; i < MAX_POINT_AMOUNT; ++i)
	{
		for (int j = 0; j < MATRIX_SIZE; ++j)
		{
			matrix[i][j] = function[j].GetTValue(i);
		}
	}

	//creating f''(0)
	for (int j = 0; j < MATRIX_SIZE; ++j)
	{
		matrix[i][j] = secondDerivative[j].GetTValue(0);
	}

	++i;

	//creating f''(k)
	for (int j = 0; j < MATRIX_SIZE; ++j)
	{
		matrix[i][j] = secondDerivative[j].GetTValue(MAX_POINT_AMOUNT - 1);
	}
}

SplineNode * SplineManager::SplineInterpolate(float t)
{
	float tInterval = t * (nodeList.size() - 1);

	unsigned int size = nodeList.size();

	Matrix matrixX(size + 2);
	Matrix matrixY(size + 2);
	for (unsigned int i = 0; i < matrixX.size(); ++i) {
		matrixX[i].resize(size + 3);
		matrixY[i].resize(size + 3);
	}

	unsigned int i = 0;
	for (; i < size; ++i)
	{
		for (unsigned int j = 0; j < size + 2; ++j)
		{
			matrixX[i][j] = coeffMatrixX[i][j];
			matrixY[i][j] = coeffMatrixY[i][j];
		}
	}

	for (unsigned int k = 0; k < size + 2;++k)
	{
		matrixX[size][k] = coeffMatrixX[MAX_POINT_AMOUNT][k];
		matrixX[size + 1][k] = coeffMatrixX[MAX_POINT_AMOUNT + 1][k];
		matrixY[size][k] = coeffMatrixY[MAX_POINT_AMOUNT][k];
		matrixY[size + 1][k] = coeffMatrixY[MAX_POINT_AMOUNT + 1][k];
	}

	for (unsigned int j = 0; j < size; ++j)
	{
		matrixX[j][size + 2] = nodeList[j]->nodeLocation.x;
		matrixY[j][size + 2] = nodeList[j]->nodeLocation.z; //In our case, y's are z's
	}

	SolveMatrix(matrixX);
	SolveMatrix(matrixY);

	i = 0;
	float x = 0, y = 0;
	for (; i < size + 2; ++i)
	{
		x += function[i].GetTValue(tInterval) * matrixX[i][size + 2];
		y += function[i].GetTValue(tInterval) * matrixY[i][size + 2];
	}
	return new SplineNode(x, 0.0f, y);

}

void SplineManager::SolveMatrix(Matrix & matrix)
{
	unsigned int row = matrix.size();
	unsigned int cols = matrix[0].size();

	for (unsigned int i = 0; i < (row - 1); i++)
	{
		if (fabsf(matrix[i][i]) < 0.0f)
		{
			for (unsigned int i2 = i + 1; i2 < row; i2++)
			{
				if (fabsf(matrix[i2][i2]) > 0.0f)
				{
					for (unsigned int j = 0; j < cols; j++)
					{
						float temp = matrix[i][j];
						matrix[i][j] = matrix[i2][j];
						matrix[i2][j] = temp;

					}
					break;
				}
			}
		}
		if (fabsf(matrix[i][i]) > 0.0f)
		{
			for (unsigned int i2 = i + 1; i2 < row; i2++)
			{
				float factor = -matrix[i2][i] / matrix[i][i];
				for (unsigned int j = 0; j < cols; j++)
				{
					matrix[i2][j] = matrix[i2][j] + factor * matrix[i][j];
				}
			}
		}
	}

	for (int r = row - 1; r >= 0; r--)
	{
		if (matrix[r][r] != 0)
			matrix[r][cols - 1] /= matrix[r][r];
		for (int r2 = r - 1; r2 >= 0; r2--)
		{
			if (matrix[r][r] != 0)
			{
				matrix[r2][cols - 1] -= (matrix[r2][r]) * matrix[r][cols - 1];
			}
		}
	}
}
