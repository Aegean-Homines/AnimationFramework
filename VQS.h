#pragma once
#include "Quaternion.h"
#include <glm.hpp>

using glm::vec3;

class VQS
{
private:
	vec3 translate;
	Quaternion rotate;
	float scalar;
public:
	VQS();
	VQS(vec3 const & translate, Quaternion const & quaternion);
	~VQS();

	VQS operator+(VQS const & rhs) const;
	VQS operator*(float const & scalar) const;
	vec3 operator*(vec3 const & translate) const;
	VQS operator*(VQS const & rhs) const;
	VQS Inverse() const;

	static VQS Slerp(VQS const & vqs1, VQS const & vqs2, float delta) {
		vec3 finalTranslate = (1.0f - delta) * vqs1.translate + delta * vqs2.translate;
		Quaternion finalRotate = Quaternion::Slerp(vqs1.rotate, vqs2.rotate, delta);

		return VQS(finalTranslate, finalRotate);
	}

};

