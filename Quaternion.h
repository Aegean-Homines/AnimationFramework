#pragma once

#include <glm.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mat3;

class Quaternion
{
private:
	float scalar;
	vec3 imaginary;
public:
	// Constructors
	// Default version, for unit Quaternion
	Quaternion(float scalar = 1.0f, float x = 0.0f, float y = 0.0f, float z = 0.0f);
	// Create Quaternion from euler angles
	Quaternion(vec3  const & rotationEulerVector);
	// Create Quaternion from vec4(scalar, imaginary)
	Quaternion(vec4 const & quaternionData);
	// Create Quaternion from scalar and vec3(imaginary)
	Quaternion(float scalar, vec3 const & imaginaryVector);
	// Create Quaternion from angle-axis 
	Quaternion(vec3 const & unitAxis, float angle);

	// Getter & Setters
	void Scalar(float s) { scalar = s; }
	float Scalar() const { return scalar; }
	void X(float x) { imaginary[0] = x; }
	float X() const { return imaginary[0]; }
	void Y(float y) { imaginary[1] = y; }
	float Y() const { return imaginary[1]; }
	void Z(float z) { imaginary[2] = z; }
	float Z() const { return imaginary[2]; }
	void Imaginary(vec3 const & imaginaryVector);
	vec3 Imaginary() const;

	// ASSIGNMENT OPERATION
	Quaternion &operator=(Quaternion const & q);

	// ARITHMETIC OPERATIONS
	Quaternion operator*(float scalar);
	Quaternion operator/(float denom);


	// QUATERNION OPERATIONS
	Quaternion operator*(Quaternion const & rhs) const;
	Quaternion operator+(Quaternion const & rhs) const;
	Quaternion operator-(Quaternion const & rhs) const;

	// UNARY NEGATE
	Quaternion operator-() const;

	// QUATERNION - MATRIX TRANSFORMATIONS
	mat4 MatrixForm() const; //matrix * rhs.vector = this * q
	mat4 MatrixFormForRightMux() const; //rhs.vector.trans * matrix = q * vector.transpose
	mat4 RotationMatrix() const;

	// QUATERNION - VECTOR TRANSFORMATIONS
	vec4 VectorForm() const; //(x,y,z,w)
	vec3 RotatedVectorForm(vec3  const & rotationVector) const; //q * q(0,v) * q.Inverse()
	vec3 EulerForm() const;

	// Extra utilities
	Quaternion Log() const;
	Quaternion Exp() const;
	void Normalize();
	
	// General Helper Functions
	static inline float Dot(Quaternion const & q1, Quaternion const & q2) {
		return ((q1.imaginary.x * q2.imaginary.x + q1.imaginary.y * q2.imaginary.y + q1.imaginary.z * q2.imaginary.z) + q1.scalar * q2.scalar);
		//return ((glm::dot(q1.imaginary, q2.imaginary)) + q1.scalar * q2.scalar);
	}

	// Length (or magnitude) of a quaternion
	static inline float Length(Quaternion const & q) {
		float length = q.scalar * q.scalar + q.imaginary.x * q.imaginary.x + q.imaginary.y * q.imaginary.y + q.imaginary.z * q.imaginary.z;
		return sqrt(length);
	}

	// Square length of a quaternion
	static inline float LengthSquared(Quaternion const & q) {
		float length = q.scalar * q.scalar + q.imaginary.x * q.imaginary.x + q.imaginary.y * q.imaginary.y + q.imaginary.z * q.imaginary.z;
		return length;
	}

	// Conjugate of a quaternion
	static inline Quaternion Conjugate(Quaternion const & q) { 
		return Quaternion(q.scalar, -(q.imaginary));
	}

	// If quaternion is unit, conjugate is faster
	static Quaternion Inverse(Quaternion const & q) {
		return Quaternion::Conjugate(q) / Quaternion::LengthSquared(q);
	}

	// Unit Quaternion
	static Quaternion Unit(Quaternion const & q) {
		return q * (1 / Quaternion::Length(q));
	}

	// Slerp for Quaternion Interpolation
	// Based on the algorithm used in Will Perone's website
	static Quaternion Slerp(Quaternion const & q1, Quaternion const & q2, float t) {
		Quaternion q3 = q2;
		float dot = Quaternion::Dot(q1, q2);

		if (dot < 0)
		{
			dot = -dot;
			q3 = -q2;
		}

		if (dot < 0.95f)
		{
			float angle = acosf(dot);
			return (q1*sinf(angle*(1 - t)) + q3*sinf(angle*t)) / sinf(angle);
		}
		else // if the angle is small, use linear interpolation								
			return Lerp(q1, q3, t);

	}

	// Linear Interpolation for quaternion (used when the angle is too narrow)
	// Based on the algorithm used in Will Perone's website
	static Quaternion Lerp(const Quaternion &q1, const Quaternion &q2, float t)
	{
		Quaternion q3 = (q1*(1 - t) + q2*t);
		q3.Normalize();
		return q3;
	}
};

