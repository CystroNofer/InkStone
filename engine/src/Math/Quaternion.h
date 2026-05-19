#pragma once

#include "Matrix.h"

namespace NXTN {
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
		static Quaternion FromRotationMatrix(mat4 r);

		std::string ToString();
	};

	void normalize(Quaternion& q);

	Quaternion operator*(const Quaternion& qa, const Quaternion& qb);
	void operator*=(Quaternion& qa, const Quaternion& qb);
}
