#include "Skeleton.h"
#include "SkeletonNode.h"
#include "TargetObject.h"
#include "Quaternion.h"
#include "VQS.h"

#include <glm.hpp>

using glm::vec3;

#define MIN_DISTANCE_TO_TARGET 0.1f
#define DELTA_EPSILON_FOR_END_EFFECTOR 0.001f
#define PI 3.14159265359f
#define MIN_ANGLE (PI/360.0f)

Skeleton::Skeleton()
{
}


Skeleton::~Skeleton()
{
	for (unsigned int i = 0; i < skeleton.size(); ++i) {
		delete skeleton[i];
	}
}

void Skeleton::RegisterJoint(SkeletonNode * newNode)
{
	skeleton.push_back(newNode);
}

SkeletonNode * Skeleton::GetJoint(unsigned int index)
{
	if (index < skeleton.size())
		return skeleton[index];
	else
		return nullptr;
}

SkeletonNode * Skeleton::GetRoot()
{
	if (skeleton.size() > 0)
		return skeleton[0];
	else
		return nullptr;
}

void Skeleton::CalculateAllTransformsUsingIK()
{
	TargetObject* target = TargetObject::Instance();

	// If no target, don't rotate
	if (!target->DoesExist())
		return;

	// If only root node don't rotate
	if (skeleton.size() < 2)
		return;

	// Local space to world space
	skeleton[1]->MoveAllToWorldSpace();

	// Get the node before the last one
	// Last one is the end effector
	SkeletonNode* endEffector = skeleton[skeleton.size() - 1];

	// End effector position
	vec3 Pc = endEffector->globalVQS.translate;
	// Target position
	vec3 Pd = target->Position();

	// will be used inside the loop
	// Vck = vector from k'th joint to Pc
	// Vdk = vector from k'th joint to Pd
	vec3 Vck, Vdk;
	vec3 Plast;
	while (glm::distance(Pc, Pd) > MIN_DISTANCE_TO_TARGET) { //#Optimization: use squared distance calculation
 		Plast = Pc;
		for (unsigned int i = skeleton.size() - 2; i > 0; --i) {
			SkeletonNode* currentJoint = skeleton[i];
			vec3 currentJointPosition = currentJoint->globalVQS.translate;

			// Set vectors from k'th joint
			Vck = glm::normalize(Pc - currentJointPosition);
			Vdk = glm::normalize(Pd - currentJointPosition);

			float cosAngle = glm::dot(Vdk, Vck);
			if (cosAngle < 0.999999f) { //which mean we rotated a little bit
				// Calculate angle and axis
				float angle = acosf(cosAngle);
				vec3 axis = glm::cross(Vck, Vdk);
				// Unit axis
				axis = glm::normalize(axis);
				// Create the quaternion to rotate the joint
				Quaternion rotator(axis, angle);
				// Update local rotation of the current joint
				Quaternion & originalRotation = currentJoint->localVQS.rotate;
				originalRotation = rotator * originalRotation;
				// Update skeleton's world coordinates starting from this
				currentJoint->MoveAllToWorldSpace();

				// Update Pc - end effector is updated when the world space is recalculated
				Pc = endEffector->globalVQS.translate;
			}
		}

		if (glm::distance(Pc, Plast) < DELTA_EPSILON_FOR_END_EFFECTOR) {
 			return;
		}
	}
}

void Skeleton::ResetAllNodes()
{
	for (unsigned int i = 0; i < skeleton.size(); ++i) {
		skeleton[i]->localVQS = skeleton[i]->initialVQS;
	}
}
