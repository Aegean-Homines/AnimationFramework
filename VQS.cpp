#include "VQS.h"

VQS::VQS(vec3 const & translate, Quaternion const & quaternion, float scalar):
	translate(translate), rotate(quaternion), scalar(scalar)
{}

VQS::VQS()
{
	translate = vec3(0.0f);
	scalar = 1.0f; // Since all scalar is 1.0f by default, I'm just setting it here
}

VQS::~VQS()
{
}

VQS VQS::operator+(VQS const & rhs) const
{
	VQS retVal;
	retVal.scalar = this->scalar + rhs.scalar;
	retVal.translate = this->translate + rhs.translate;
	retVal.rotate = this->rotate + rhs.rotate;
	return retVal;
}

VQS VQS::operator*(float const & scalar) const
{
	VQS retVal;
	retVal.scalar *= scalar;
	retVal.translate *= scalar;
	retVal.rotate = retVal.rotate * scalar;
	return retVal;
}

VQS VQS::operator*(VQS const & rhs) const
{
	vec3 translatePart = *this * rhs.translate;
	Quaternion quatPart = rotate * rhs.rotate;
	float scalarPart = scalar * rhs.scalar;

	return VQS(translatePart, quatPart, scalarPart);
}

vec3 VQS::operator*(vec3 const & translate) const
{
	vec3 scaledTranslate = translate * scalar;
	Quaternion quatFromTranslate(0, scaledTranslate); //pure quaternion
	Quaternion inverse = Quaternion::Inverse(rotate);
	Quaternion firstPart = rotate * quatFromTranslate * inverse;
	return (firstPart.Imaginary() + this->translate);
}

VQS VQS::Inverse() const
{
	vec3 scaledTranslate = translate / scalar;
	Quaternion quatFromTranslate(0, scaledTranslate);
	Quaternion firstPart = (-Quaternion::Inverse(rotate) * quatFromTranslate * rotate);
	vec3 translatePart = firstPart.Imaginary();
	Quaternion secondPart = Quaternion::Inverse(rotate);

	return VQS(translatePart, secondPart);
}
