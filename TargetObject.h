#pragma once

#include <glm.hpp>
#include "ShaderProgram.h"

using glm::vec3;

class Mesh;

// Just a representation of the target object in the scene
class TargetObject
{
public:
	~TargetObject();

	static TargetObject* Instance();

	void SetPosition(vec3 newposition) { position = newposition; }
	void SetRotation(vec3 newrotation) { rotation = newrotation; }
	void ScaleObject(vec3 newScale) { scale = newScale; }
	void SetMesh(Mesh* targetMesh) { myMesh = targetMesh; }
	void SetColor(vec3 newColor) { color = newColor; }
	void SetLastUpdatedPosition(vec3 pos) { lastUpdatedPosition = pos; }

	void MoveObject(vec3 deltaPosition) { position += deltaPosition; }
	void RotateObject(vec3 deltaRotation) { rotation += deltaRotation; }
	vec3 const & Position() const { return position; }
	vec3 const & LastUpdatedPosition() const { return lastUpdatedPosition; }

	// For drawing a cube as a target
	void Draw(ShaderProgram const & program);
	void Update(); //For mouse controls

	static bool DoesExist() { return static_cast<bool>(instance); }
private:
	Mesh* myMesh;
	vec3 position;
	vec3 lastUpdatedPosition;
	vec3 rotation;
	vec3 scale;
	vec3 color;

	bool isPossessed = false;
	static TargetObject* instance;

	TargetObject(vec3 position = vec3(0.0f), vec3 rotation = vec3(0.0f), vec3 scale = vec3(1.0f), vec3 color = vec3(1.0f));
};

