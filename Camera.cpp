#include "EventManager.h"
#include "EngineHelper.h"
#include "Camera.h"
#include <gtc\matrix_transform.hpp>
#include <glfw3.h>

#include <iostream>

#define CAMERA_SPEED 5.0f
#define MOUSE_SENSITIVITY 0.10f

const vec3 up(0.0f, 1.0f, 0.0f);

Camera::Camera(vec3 cameraPosition /*= vec3(0.0f, 0.0f, 0.0f)*/, vec3 cameraTarget /*= vec3(0.0f, 0.0f, 0.0f)*/) :
	cameraPosition(cameraPosition), cameraTarget(cameraTarget), cameraFront(0.0f, 0.0f, -1.0f)
{
	yaw = -90.0f;
	pitch = 0.0f;
}

Camera::~Camera()
{
}

mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(cameraPosition, cameraPosition + cameraFront, up);
}

void Camera::Update()
{
	float cameraSpeed;
	if (EventManager::IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
		cameraSpeed = CAMERA_SPEED * 3.0f;
	else
		cameraSpeed = CAMERA_SPEED;

	cameraSpeed *= static_cast<float>(EngineHelper::DeltaTime());

	if (EventManager::IsKeyPressed(GLFW_KEY_W))
		cameraPosition += cameraSpeed * cameraFront;
	if (EventManager::IsKeyPressed(GLFW_KEY_S))
		cameraPosition -= cameraSpeed * cameraFront;
	if (EventManager::IsKeyPressed(GLFW_KEY_A))
		cameraPosition -= glm::normalize(glm::cross(cameraFront, up)) * cameraSpeed;
	if (EventManager::IsKeyPressed(GLFW_KEY_D))
		cameraPosition += glm::normalize(glm::cross(cameraFront, up)) * cameraSpeed;

	// rotation
	if (EventManager::IsMouseButtonPressed(RIGHT)) {
		yaw += static_cast<float>(EventManager::MouseDeltaX()) * MOUSE_SENSITIVITY;
		pitch += static_cast<float>(EventManager::MouseDeltaY()) * MOUSE_SENSITIVITY;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch <= -89.0f)
			pitch = -89.0f;

		vec3 front;
		front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		front.y = sin(glm::radians(pitch));
		front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

		cameraFront = glm::normalize(front);
	}

}
