#pragma once

#include "Matrix.h"

namespace NXTN {
	const long double CONST_PI = 3.14159265358979323846264338327950288419716;
	const long double CONST_PI_OVER_180 = 0.01745329251994329576923690768489;

	// Angle in radius
	struct Quaternion
	{
		float w, x, y, z;

		inline Quaternion() : w(1), x(0), y(0), z(0) {}
		inline Quaternion(float w0, float x0, float y0, float z0) : w(w0), x(x0), y(y0), z(z0) {}
		Quaternion(float angle, const vec3& axis);
		~Quaternion() {}

		Quaternion Conjugate();
		float Length();

		void Rotate(float angle, const vec3& axis);
		vec3 ApplyRotation(const vec3& vector);

		mat4 GetRotationMatrix();

		std::string ToString();
	};

	Quaternion operator*(const Quaternion& qa, const Quaternion& qb);
	void operator*=(Quaternion& qa, const Quaternion& qb);
}
