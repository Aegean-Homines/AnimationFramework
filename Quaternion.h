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
	Quaternion(float scalar = 1.0f, float x = 0.0f, float y = 0.0f, float z = 0.0f);
	Quaternion(vec3  const & rotationEulerVector);
	Quaternion(vec4 const & quaternionData);
	Quaternion(float scalar, vec3 const & imaginaryVector);
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

	// ARITHMETIC OPERATIONS
	Quaternion operator*(float scalar);
	Quaternion operator/(float denom);


	// QUATERNION OPERATIONS
	Quaternion operator*(Quaternion const & rhs) const;
	Quaternion operator+(Quaternion const & rhs) const;
	Quaternion operator-(Quaternion const & rhs) const;
	Quaternion operator-();


	// QUATERNION - MATRIX
	mat4 MatrixForm() const; //matrix * rhs.vector = this * q
	mat4 MatrixFormForRightMux() const; //rhs.vector.trans * matrix = q * vector.transpose
	mat4 RotationMatrix() const;

	// QUATERNION - VECTOR
	vec4 VectorForm() const; //(x,y,z,w)
	vec3 RotatedVectorForm(vec3  const & rotationVector) const; //q * q(0,v) * q.Inverse()
	vec3 EulerForm() const;

	// Extra utilities
	Quaternion Log() const;
	Quaternion Exp() const;
	void Normalize();
	
	// General Helper Functions
	static inline float Dot(Quaternion const & q1, Quaternion const & q2) {
		return (glm::dot(q1.imaginary, q2.imaginary) + q1.scalar * q2.scalar);
	}

	// Length (or magnitude) of a quaternion
	static inline float Length(Quaternion const & q) {
		float length = q.scalar * q.scalar + q.imaginary.x * q.imaginary.x + q.imaginary.y * q.imaginary.y + q.imaginary.z * q.imaginary.z;
		return sqrt(length);
	}

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

	// #TODO LERP, SLERP ETC

};

