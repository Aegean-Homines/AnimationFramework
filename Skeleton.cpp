#include "Skeleton.h"
#include "SkeletonNode.h"
#include "TargetObject.h"
#include "Quaternion.h"
#include "VQS.h"
#include "AngleMath.h"

#include <glm.hpp>
#include <iostream>

using glm::vec3;

#define MIN_DISTANCE_TO_TARGET 0.01f
#define DELTA_EPSILON_FOR_END_EFFECTOR 0.1f

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
	skeleton[0]->MoveAllToWorldSpace();

	// Get the node before the last one
	// Last one is the end effector
	SkeletonNode* endEffector = skeleton[skeleton.size() - 1];

	std::vector<SkeletonNode*> nodePriorityList;
	// Order nodes w.r.t a priority - we can add more options later
	OrderNodes(skeleton, nodePriorityList, NONE);

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
		for ( int i = nodePriorityList.size() - 2; i >= 0; --i) {
			SkeletonNode* currentJoint = nodePriorityList[i];
			vec3 currentJointPosition = currentJoint->localVQS.translate;

			// I need to take everything to the local space of the current node
			// I'm still not sure why though but nothings working when I do the calculations on the global space
			vec3 PcInv(glm::inverse(currentJoint->transformMatrix) * vec4(Pc, 1.0f));
			vec3 PdInv(glm::inverse(currentJoint->transformMatrix) * vec4(Pd, 1.0f));

			// Set vectors from k'th joint
			Vck = glm::normalize(PcInv - currentJointPosition);
			Vdk = glm::normalize(PdInv - currentJointPosition);

			float cosAngle = glm::dot(Vdk, Vck);
			if (cosAngle < 0.9999f) { //which mean we rotated a little bit
				// Calculate angle and axis
				float angle = acosf(cosAngle);
				vec3 axis = glm::cross(Vck, Vdk);
				// Unit axis
				axis = glm::normalize(axis);

				// Angle check - damping the rotation
				float currentDampening = currentJoint->GetDampening(); //in degrees
				currentDampening = DEGTORAD(currentDampening);
				if (angle > currentDampening)
					angle = currentDampening;

				// Create the quaternion to rotate the joint
				Quaternion rotator(axis, angle);
				// Update local rotation of the current joint
				Quaternion & originalRotation = currentJoint->localVQS.rotate;
				originalRotation = originalRotation * rotator;

				//DOF restrictions
				vec3 rotationEuler = originalRotation.EulerForm(); //degree
				float maxAngle = currentJoint->GetMaxAngle();
				float minAngle = currentJoint->GetMinAngle();

				ClampVector(rotationEuler, maxAngle, minAngle);
				/*if (rotationEuler.x > maxAngle)
					rotationEuler.x = maxAngle;
				if (rotationEuler.x < minAngle)
					rotationEuler.x = minAngle;
				if (rotationEuler.y > maxAngle)
					rotationEuler.y = maxAngle;
				if (rotationEuler.y < minAngle)
					rotationEuler.y = minAngle;
				if (rotationEuler.z > maxAngle)
					rotationEuler.z = maxAngle;
				if (rotationEuler.z < minAngle)
					rotationEuler.z = minAngle;*/

				originalRotation = Quaternion(rotationEuler);

				originalRotation.Normalize();

				// Update skeleton's world coordinates starting from this
				currentJoint->MoveAllToWorldSpace();

				// Update Pc with its new global value
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

void Skeleton::OrderNodes(vector<SkeletonNode*> const & skeleton, vector<SkeletonNode*> & orderedList, OrderingTechnique technique)
{
	orderedList.clear();

	switch (technique)
	{
	case OrderingTechnique::REVERSE:
		for (unsigned int i = skeleton.size() - 1; i != 0; --i) {
			orderedList.push_back(skeleton[i]);
		}
		break;
	case OrderingTechnique::NONE:
	default:
		orderedList.insert(orderedList.begin(), skeleton.begin(), skeleton.end());
		break;
	}
}
