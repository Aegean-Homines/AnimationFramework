#pragma once
#include "Quaternion.h"
#include <glm.hpp>

using glm::vec3;

class VQS
{
public:
	// Constructors
	VQS();
	VQS(vec3 const & translate, Quaternion const & quaternion, float scalar = 1.0f);
	~VQS();

	// VQS addition
	VQS operator+(VQS const & rhs) const;

	// Scalar multiplication
	VQS operator*(float const & scalar) const;

	// VQS * vector
	vec3 operator*(vec3 const & translate) const;

	// VQS * VQS
	VQS operator*(VQS const & rhs) const;

	// Inverse of this VQS
	VQS Inverse() const;

	// Lerp of translate and Slerp of Quaternion
	static VQS Slerp(VQS const & vqs1, VQS const & vqs2, float delta) {
		vec3 finalTranslate = (1.0f - delta) * vqs1.translate + delta * vqs2.translate;
		Quaternion finalRotate = Quaternion::Slerp(vqs1.rotate, vqs2.rotate, delta);
		finalRotate.Normalize();
		float finalScale = pow(vqs2.scalar / vqs1.scalar, delta) * vqs1.scalar;
		return VQS(finalTranslate, finalRotate, finalScale);
	}

	//data
	vec3 translate;
	Quaternion rotate;
	float scalar;

};

