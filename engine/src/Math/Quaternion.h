#pragma once

#include "Matrix.h"

namespace NXTN {
	// Angle in radius
	struct Quaternion
	{
		float x, y, z, w;

		inline Quaternion() : x(0), y(0), z(0), w(1) {}
		inline Quaternion(float x0, float y0, float z0, float w0) : x(x0), y(y0), z(z0), w(w0) {}
		Quaternion(float angle, const vec3& axis);
		~Quaternion() {}

		Quaternion Conjugate();
		float Length();

		void Rotate(float angle, const vec3& axis);
		vec3 ApplyRotation(const vec3& vector);

		mat4 GetRotationMatrix();
		static Quaternion FromRotationMatrix(mat4 r);

		std::string ToString();
	};

	void normalize(Quaternion& q);

	Quaternion operator*(const Quaternion& qa, const Quaternion& qb);
	void operator*=(Quaternion& qa, const Quaternion& qb);
}
