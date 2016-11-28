#pragma once

#include <glm.hpp>
#include "ShaderProgram.h"

using glm::vec3;

class Mesh;

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

	void MoveObject(vec3 deltaPosition) { position += deltaPosition; }
	void RotateObject(vec3 deltaRotation) { rotation += deltaRotation; }
	vec3 const & Position() const { return position; }

	void Draw(ShaderProgram const & program);
	void Update();

	static bool DoesExist() { return static_cast<bool>(instance); }
private:
	Mesh* myMesh;
	vec3 position;
	vec3 rotation;
	vec3 scale;
	vec3 color;

	bool isPossessed = false;
	static TargetObject* instance;

	TargetObject(vec3 position = vec3(0.0f), vec3 rotation = vec3(0.0f), vec3 scale = vec3(1.0f), vec3 color = vec3(1.0f));
};

