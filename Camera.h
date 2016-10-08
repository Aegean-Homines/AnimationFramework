#pragma once

#include <glm.hpp>

using glm::vec3;
using glm::mat4;

class Camera
{
public:
	Camera(vec3 cameraPosition = vec3(0.0f, 0.0f, 0.0f), vec3 cameraTarget = vec3(0.0f, 0.0f, 0.0f));
	~Camera();

	mat4 GetViewMatrix();
	vec3 const & CameraPosition() const { return cameraPosition; }
	void CameraPosition(vec3 const & newCameraPosition) { cameraPosition = newCameraPosition; }
	vec3 const & CameraTarget() const { return cameraTarget; }
	void CameraTarget(vec3 const & newCameraTarget) { cameraTarget = newCameraTarget; }

	void Update();
private:
	vec3 cameraPosition;
	vec3 cameraTarget;
	vec3 cameraFront;

	float yaw, pitch;
};

