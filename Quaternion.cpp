#include "Quaternion.h"

#ifndef PI
#define PI 3.14159265359f
#endif

const static float PiOver2 = PI * 0.5f;
const static float EpsilonForEuler = 0.00000001f;
const float angleMultiplication = (PI / 180.0f);

Quaternion::Quaternion(float scalar /*= 1.0f*/, float x /*= 1.0f*/, float y /*= 1.0f*/, float z /*= 1.0f*/):
	scalar(scalar), imaginary(x,y,z)
{
}

Quaternion::Quaternion(vec4 const & data): 
	scalar(data[0]), imaginary(data[1], data[2], data[3])
{
}

Quaternion::Quaternion(float scalar, vec3 const & imaginaryVector)
{
	imaginary = imaginaryVector;
	this->scalar = scalar;
}

Quaternion::Quaternion(vec3 const & rotationEulerVector)
{
	vec3 convertedEulerVector = rotationEulerVector * angleMultiplication;

	float cosX = glm::cos(convertedEulerVector[0] * 0.5f);
	float cosY = glm::cos(convertedEulerVector[1] * 0.5f);
	float cosZ = glm::cos(convertedEulerVector[2] * 0.5f);

	float sinX = glm::sin(convertedEulerVector[0] * 0.5f);
	float sinY = glm::sin(convertedEulerVector[1] * 0.5f);
	float sinZ = glm::sin(convertedEulerVector[2] * 0.5f);

	imaginary[0] = cosZ * cosY * sinX - sinZ * sinY * cosX;
	imaginary[1] = cosZ * sinY * cosX + sinZ * cosY * sinX;
	imaginary[2] = sinZ * cosY * cosX - cosZ * sinY * sinX;
	scalar = cosZ * cosY * cosX + sinZ * sinY * sinX;

}

Quaternion::Quaternion(vec3 const & unitAxis, float angle)
{
	float convertedAngle = angle * angleMultiplication;

	scalar = cosf(convertedAngle / 2.0f);
	imaginary = unitAxis * sinf(convertedAngle / 2.0f);
}

void Quaternion::Imaginary(vec3 const & imaginaryVector)
{
	imaginary[0] = imaginaryVector[0];
	imaginary[1] = imaginaryVector[1];
	imaginary[2] = imaginaryVector[2];
}

vec3 Quaternion::Imaginary() const
{
	return vec3(imaginary);
}

Quaternion& Quaternion::operator=(Quaternion const & q)
{
	scalar = q.scalar;
	imaginary = q.imaginary;
	return *this;
}

Quaternion Quaternion::operator/(float denom)
{
	Quaternion resultingQuaternion(Scalar() / denom, Imaginary() / denom);
	return resultingQuaternion;
}

Quaternion Quaternion::operator*(Quaternion const & rhs) const
{
	float scalarTerm;
	float imaginaryX, imaginaryY, imaginaryZ;

	scalarTerm = scalar * rhs.scalar - (imaginary.x * rhs.imaginary.x + imaginary.y * rhs.imaginary.y + imaginary.z * rhs.imaginary.z);
	imaginaryX = imaginary.y * rhs.imaginary.z - imaginary.z * rhs.imaginary.y + scalar * rhs.imaginary.x + imaginary.x * rhs.scalar;
	imaginaryY = imaginary.z * rhs.imaginary.x - imaginary.x * rhs.imaginary.z + scalar * rhs.imaginary.y + imaginary.y * rhs.scalar;
	imaginaryZ = imaginary.x * rhs.imaginary.y - imaginary.y * rhs.imaginary.x + scalar * rhs.imaginary.z + imaginary.z * rhs.scalar;

	return Quaternion(scalarTerm, imaginaryX, imaginaryY, imaginaryZ);
}

Quaternion Quaternion::operator*(float scalar)
{
	return Quaternion(this->scalar * scalar, imaginary * scalar);
}

Quaternion Quaternion::operator+(Quaternion const & rhs) const
{
	return Quaternion(scalar + rhs.scalar, imaginary + rhs.imaginary);
}

Quaternion Quaternion::operator-(Quaternion const & rhs) const
{
	float x, y, z;
	x = imaginary[0] - rhs.imaginary[0];
	y = imaginary[1] - rhs.imaginary[1];
	z = imaginary[2] - rhs.imaginary[2];

	return Quaternion(scalar - rhs.scalar, x, y, z);
}

Quaternion Quaternion::operator-() const
{
	float x, y, z;
	x = -imaginary[0];
	y = -imaginary[1];
	z = -imaginary[2];

	return Quaternion(-scalar, x, y, z);
}

mat4 Quaternion::MatrixForm() const
{
	// Diagonal = scalar
	mat4 matrixForm(scalar);

	float x, y, z;
	x = imaginary[0];
	y = imaginary[1];
	z = imaginary[2];

	// First column
	matrixForm[0][1] = z;
	matrixForm[0][2] = -y;
	matrixForm[0][3] = -x;
	// Second column
	matrixForm[1][0] = -z;
	matrixForm[1][2] = x;
	matrixForm[1][3] = -y;
	// Third Column
	matrixForm[2][0] = y;
	matrixForm[2][1] = -x;
	matrixForm[2][3] = -z;
	// Fourth Column
	matrixForm[3][0] = x;
	matrixForm[3][1] = y;
	matrixForm[3][2] = z;

	return matrixForm;
}

mat4 Quaternion::MatrixFormForRightMux() const
{
	// Diagonal = scalar
	mat4 matrixForm(Scalar());

	float x, y, z;
	x = imaginary[0];
	y = imaginary[1];
	z = imaginary[2];

	// First column
	matrixForm[0][1] = z;
	matrixForm[0][2] = -y;
	matrixForm[0][3] = x;
	// Second column
	matrixForm[1][0] = -z;
	matrixForm[1][2] = x;
	matrixForm[1][3] = y;
	// Third Column
	matrixForm[2][0] = y;
	matrixForm[2][1] = -x;
	matrixForm[2][3] = z;
	// Fourth Column
	matrixForm[3][0] = -x;
	matrixForm[3][1] = -y;
	matrixForm[3][2] = -z;

	return matrixForm;
}

mat4 Quaternion::RotationMatrix() const
{
	mat4 rotationMatrix(1.0f);

	float x, y, z;
	x = imaginary[0];
	y = imaginary[1];
	z = imaginary[2];

	// First Column
	rotationMatrix[0][0] = 1 - 2 * (y*y + z*z);
	rotationMatrix[0][1] = 2 * (x*y + z*scalar);
	rotationMatrix[0][2] = 2 * (x*z - y*scalar);
	rotationMatrix[0][3] = 0.0f;
	// Second Column
	rotationMatrix[1][0] = 2 * (x*y - z*scalar);
	rotationMatrix[1][1] = 1 - 2 * (x*x + z*z);
	rotationMatrix[1][2] = 2 * (y*z + x*scalar);
	rotationMatrix[1][3] = 0.0f;

	// Third Column
	rotationMatrix[2][0] = 2 * (x*z + y*scalar);
	rotationMatrix[2][1] = 2 * (y*z - x*scalar);
	rotationMatrix[2][2] = 1 - 2 * (x*x + y*y);
	rotationMatrix[2][3] = 0.0f;

	// Fourth Column
	rotationMatrix[3][0] = 0.0f;
	rotationMatrix[3][1] = 0.0f;
	rotationMatrix[3][2] = 0.0f;
	rotationMatrix[3][3] = 1.0f;

	return rotationMatrix;
}

vec4 Quaternion::VectorForm() const
{
	return vec4(imaginary, scalar);
}

vec3 Quaternion::RotatedVectorForm(vec3 const & rotationVector) const
{
	Quaternion pureQuaternion(0, rotationVector);
	Quaternion conjugateQuaternion = Quaternion::Conjugate(*this);

	return ((*this) * pureQuaternion * conjugateQuaternion).Imaginary();
}

// Taken From Standford Quaternion Header file
vec3 Quaternion::EulerForm() const
{
	vec3 euler;
	float x, y, z;
	x = imaginary[0];
	y = imaginary[1];
	z = imaginary[2];

	float xSquare, ySquare, zSquare, scalarSquare;
	xSquare = imaginary[0] * imaginary[0];
	ySquare = imaginary[1] * imaginary[1];
	zSquare = imaginary[2] * imaginary[2];
	scalarSquare = scalar * scalar;

	euler[1] = static_cast<float>(asin(2.0 * (scalar * y - x * z)));
	if (PiOver2 - fabs(euler[1]) > EpsilonForEuler) {
		euler[2] = static_cast<float>(atan2(2.0 * (x * y + scalar * z), xSquare - ySquare - zSquare + scalarSquare));
		euler[0] = static_cast<float>(atan2(2.0 * (scalar * x + y * z), scalarSquare - xSquare - ySquare + zSquare));
	}
	else {
		euler[2] = atan2(2 * y * z - 2 * x * scalar,
			2 * x * z + 2 * y * scalar);
		euler[0] = 0.0;

		// If facing down, reverse yaw
		if (euler[1] < 0)
			euler[2] = PI - euler[2];
	}
	return euler;
}

// Based on the algorithm used in Will Perone's website
Quaternion Quaternion::Log() const
{
	float arcCosScalar = acos(Scalar());
	float sinArcCosScalar = sin(arcCosScalar);

	Quaternion returnValue;
	
	returnValue.Scalar(0.0f);
	if (sinArcCosScalar > 0) {
		returnValue.imaginary[0] = arcCosScalar * (imaginary[0] / sinArcCosScalar);
		returnValue.imaginary[1] = arcCosScalar * (imaginary[1] / sinArcCosScalar);
		returnValue.imaginary[2] = arcCosScalar * (imaginary[2] / sinArcCosScalar);
	}
	else {
		returnValue.imaginary[0] = returnValue.imaginary[1] = returnValue.imaginary[2] = 0.0f;
	}

	return returnValue;
}

// Based on the algorithm used in Will Perone's website
Quaternion Quaternion::Exp() const
{
	float lengthOfImaginary = glm::length(Imaginary());
	float sinOfLength = sin(lengthOfImaginary);
	float cosOfLength = cos(lengthOfImaginary);

	Quaternion returnValue;
	returnValue.Scalar(cosOfLength);
	if (lengthOfImaginary > 0) {
		returnValue.imaginary.x = sinOfLength * (imaginary.x / lengthOfImaginary);
		returnValue.imaginary.y = sinOfLength * (imaginary.y / lengthOfImaginary);
		returnValue.imaginary.z = sinOfLength * (imaginary.z / lengthOfImaginary);
	}
	else {
		returnValue.imaginary.x = returnValue.imaginary.y = returnValue.imaginary.z = 0.0f;
	}

	return returnValue;
}

void Quaternion::Normalize()
{
	float length = Quaternion::Length(*this);
	*this = *this / length;
}