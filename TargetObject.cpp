#include "TargetObject.h"
#include "EventManager.h"
#include "SplineManager.h"
#include "Skeleton.h"
#include "Camera.h"
#include "GraphicsManager.h"
#include "ArcLengthSegmentManager.h"
#include "Mesh.h"

#include <iostream>
#include <glfw3.h>
#include <glew.h>
#include <gtc/matrix_transform.hpp>

#define TARGET_SPEED 0.05f
#define USE_CAMERA_SPACE true

using glm::mat4;
TargetObject* TargetObject::instance = nullptr;

TargetObject::TargetObject(vec3 position, vec3 rotation, vec3 scale, vec3 color):
	position(position), rotation(rotation), scale(scale), color(color)
{
	lastUpdatedPosition = position;
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
	if (EventManager::IsKeyPressed(GLFW_KEY_SPACE)) {
		isPossessed = true;

// USE_CAMERA_SPACE: If enabled, it moves the target object with respect to camera viewport
// If not, it uses world space directions to handle the movement - a bit confusing for my taste
#if USE_CAMERA_SPACE
		Camera& camera = GraphicsManager::GetCamera();
		vec3 const & up = camera.GetUpVector();
		vec3 const & forward = camera.GetForwardVector();
		vec3 const & right = camera.GetRightVector();
#endif

		// Mouse control
		if (EventManager::IsMouseButtonPressed(RIGHT)) {
			double wheelY = EventManager::WheelOffsetY();

			if (EventManager::MouseDeltaX() || EventManager::MouseDeltaY() || wheelY) {
				isPossessed = true;
			}
#if USE_CAMERA_SPACE

			position += static_cast<float>(EventManager::MouseDeltaX()) * TARGET_SPEED * right;
			position += static_cast<float>(EventManager::MouseDeltaY()) * TARGET_SPEED * forward;
			position += static_cast<float>(wheelY) * TARGET_SPEED * up * 4.0f;
#else
			position.x += static_cast<float>(EventManager::MouseDeltaX()) * TARGET_SPEED;
			position.y += static_cast<float>(EventManager::MouseDeltaY()) * TARGET_SPEED;
			position.z += static_cast<float>(wheelY) * TARGET_SPEED;
#endif
		
		}
		else { // Keyboard control
#if USE_CAMERA_SPACE
			if (EventManager::IsKeyTriggered(GLFW_KEY_UP)) {
				position += (TARGET_SPEED * forward * 10.0f);
				
			}

			if (EventManager::IsKeyTriggered(GLFW_KEY_DOWN)) {
				position -= (TARGET_SPEED * forward * 10.0f);
				//isPossessed = true;
			}

			if (EventManager::IsKeyTriggered(GLFW_KEY_LEFT)) {
				position -= (right * TARGET_SPEED * 10.0f);
				//isPossessed = true;
			}

			if (EventManager::IsKeyTriggered(GLFW_KEY_RIGHT)) {
				position += (right * TARGET_SPEED * 10.0f);
				//isPossessed = true;
			}

			if (EventManager::IsKeyTriggered(GLFW_KEY_LEFT_SHIFT)) {
				position += (TARGET_SPEED * up * 10.0f);
				//isPossessed = true;
			}

			if (EventManager::IsKeyTriggered(GLFW_KEY_LEFT_CONTROL)) {
				position -= (TARGET_SPEED * up * 10.0f);
				//isPossessed = true;
			}
#else
			if (EventManager::IsKeyTriggered(GLFW_KEY_UP)) {
				position.z += (TARGET_SPEED * 10.0f);
				isPossessed = true;
			}

			if (EventManager::IsKeyTriggered(GLFW_KEY_DOWN)) {
				position.z -= (TARGET_SPEED * 10.0f);
				//isPossessed = true;
			}

			if (EventManager::IsKeyTriggered(GLFW_KEY_LEFT)) {
				position.x -= (TARGET_SPEED * 10.0f);
				//isPossessed = true;
			}

			if (EventManager::IsKeyTriggered(GLFW_KEY_RIGHT)) {
				position.x += (TARGET_SPEED * 10.0f);
				//isPossessed = true;
			}

			if (EventManager::IsKeyTriggered(GLFW_KEY_LEFT_SHIFT)) {
				position.y += (TARGET_SPEED * 10.0f);
				//isPossessed = true;
			}

			if (EventManager::IsKeyTriggered(GLFW_KEY_LEFT_CONTROL)) {
				position.y -= (TARGET_SPEED * 10.0f);
				//isPossessed = true;
			}
#endif
		}

	}

	if (isPossessed) {
		if (lastUpdatedPosition != position) {
			// It changed its position = re-build everything
			SplineManager* instance = SplineManager::Instance();
			instance->TargetObjectNewMoved(position); //Re-initialize splines
			ArcLengthSegmentManager* segmentManager = ArcLengthSegmentManager::Instance();
			segmentManager->FillSegmentationTable(instance->alpha, 0.0001f);
		}

		lastUpdatedPosition = position;
		isPossessed = false;
	}

	/*else if (EventManager::IsKeyReleased(GLFW_KEY_SPACE) && isPossessed) {
		
		if(lastUpdatedPosition != position)
			instance->TargetObjectNewMoved(position); //Re-initialize splines
		
		lastUpdatedPosition = position;
		isPossessed = false;
	}*/
}
