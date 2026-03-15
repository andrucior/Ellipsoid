#pragma once
#include "cmath"
#include <iostream>

class Vector3 {
public:
	float x, y, z;
	Vector3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {};

	Vector3 operator+(const Vector3& v) const {
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator+(float scalar) const {
		return Vector3(x + scalar, y + scalar, z + scalar);
	}

	Vector3 operator*(float scalar) const {
		return Vector3(scalar * x, scalar * y, scalar * z);
	}

	Vector3 operator-(const Vector3& v) const {
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	float operator*(const Vector3& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	Vector3 operator-() const { 
		return { -x, -y, -z };
	}

	Vector3 cross(const Vector3& v) const {
		return Vector3(
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x
		);
	}

	float dot(const Vector3& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	float length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	Vector3 normalize() const {
		float len = length();

		if (len == 0) return Vector3();
		
		return (*this) * (1 / len);
	}

	void print() const {
		std::cout << "(" << x << ", " << y << ", " << z << ")\n";
	}
};