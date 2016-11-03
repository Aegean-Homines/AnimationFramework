#pragma once

#include "ShaderProgram.h"

#include <glm.hpp>
#include <list>

using glm::vec3;
using std::list;

struct SplineNode {
	vec3 nodeLocation;
	SplineNode(vec3 const & location): nodeLocation(location){}
	SplineNode(float x, float y, float z): nodeLocation(vec3(x,y,z)){}
};

typedef std::list<SplineNode*> SplineNodeList;
typedef std::list<SplineNode*>::iterator SplineNodeListIterator;

class SplineManager
{
public:
	static SplineManager* Instance();
	~SplineManager();

	void DrawSpline(ShaderProgram program);
	void InsertNode(SplineNode* node);
private:
	SplineNodeList nodeList;
	void DrawConnectionBetweenNodes(ShaderProgram program);

	SplineManager();
	SplineManager(SplineManager const &) {};
	SplineManager& operator=(SplineManager const &) {};
	static SplineManager* instance;
};

