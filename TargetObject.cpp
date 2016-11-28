#include "TargetObject.h"
#include "EventManager.h"
#include "SplineManager.h"
#include "Skeleton.h"
#include "Mesh.h"

#include <iostream>
#include <glfw3.h>
#include <glew.h>
#include <gtc/matrix_transform.hpp>

#define TARGET_SPEED 0.5f

using glm::mat4;
TargetObject* TargetObject::instance = nullptr;

TargetObject::TargetObject(vec3 position, vec3 rotation, vec3 scale, vec3 color):
	position(position), rotation(rotation), scale(scale), color(color)
{
}

TargetObject::~TargetObject()
{
}

TargetObject * TargetObject::Instance()
{
	if (!instance)
		instance = new TargetObject();

	return instance;
}

void TargetObject::Draw(ShaderProgram const & program)
{
	// Reset matrix to identity
	glm::mat4 worldTransformation = mat4(1.0f);
	// Translate, rotate and scale

	worldTransformation = glm::translate(worldTransformation, position);
	worldTransformation = glm::rotate(worldTransformation, rotation.z, vec3(0.0f, 0.0f, 1.0f));
	worldTransformation = glm::rotate(worldTransformation, rotation.y, vec3(0.0f, 1.0f, 0.0f));
	worldTransformation = glm::rotate(worldTransformation, rotation.x, vec3(1.0f, 0.0f, 0.0f));
	worldTransformation = glm::scale(worldTransformation, scale);

	GLint transformLocation = glGetUniformLocation(program.program, "Transform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &(worldTransformation[0][0]));

	GLint colorLocation = glGetUniformLocation(program.program, "Color");
	glUniform3fv(colorLocation, 1, &color[0]);

	myMesh->Draw(program);
}

void TargetObject::Update()
{
    SplineManager* instance = SplineManager::Instance();
	if (EventManager::IsKeyPressed(GLFW_KEY_SPACE)) {
		isPossessed = true;
		if (EventManager::IsKeyTriggered(GLFW_KEY_UP)) {
			position.z -= TARGET_SPEED;
		}

		if (EventManager::IsKeyTriggered(GLFW_KEY_DOWN)) {
			position.z += TARGET_SPEED;
		}

		if (EventManager::IsKeyTriggered(GLFW_KEY_LEFT)) {
			position.x -= TARGET_SPEED;
		}

		if (EventManager::IsKeyTriggered(GLFW_KEY_RIGHT)) {
			position.x += TARGET_SPEED;
		}
	}
	else if (EventManager::IsKeyReleased(GLFW_KEY_SPACE) && isPossessed) {
		std::cout << "ISKEYRELEASED" << std::endl;
		instance->TargetObjectNewMoved(position); //Re-initialize splines
		
		isPossessed = false;
	}
}
