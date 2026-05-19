#include "pch.h"

#include "Quaternion.h"

namespace NXTN {
	Quaternion::Quaternion(float angle, const vec3& axis)  // Angle in radius
	{
		angle /= 2.0f;

		w = std::cos(angle);

		float sinHTheta = std::sin(angle);
		vec3 n_axis = normalize(axis);

		x = sinHTheta * n_axis.x;
		y = sinHTheta * n_axis.y;
		z = sinHTheta * n_axis.z;
	}

	Quaternion Quaternion::Conjugate()
	{
		return Quaternion(w, -x, -y, -z);
	}

	float Quaternion::Length()
	{
		return std::sqrt(w * w + x * x + y * y + z * z);
	}

	void Quaternion::Rotate(float angle, const vec3& axis)  // Angle in radius
	{
		angle /= 2.0f;

		float w0 = std::cos(angle);

		float sinHTheta = std::sin(angle);
		vec3 n_axis = normalize(axis);

		float x0 = sinHTheta * n_axis.x;
		float y0 = sinHTheta * n_axis.y;
		float z0 = sinHTheta * n_axis.z;

		w = w * w0 - x * x0 - y * y0 - z * z0;
		x = w * x0 + x * w0 + y * z0 - z * y0;
		y = w * y0 - x * z0 + y * w0 + z * x0;
		y = w * z0 + x * y0 - y * x0 + z * w0;
	}

	vec3 Quaternion::ApplyRotation(const vec3& vector)
	{
		vec3 res = vector;
		vec3 q(x, y, z);

		res *= (w * w - x * x - y * y - z * z);
		res += 2 * dot(q, vector) * q;
		res += 2 * w * cross(q, vector);

		return res;
	}

	mat4 Quaternion::GetRotationMatrix()
	{
		float r11 = 1.0f - 2.0f * (y * y + z * z);
		float r12 = 2.0f * (x * y - w * z);
		float r13 = 2.0f * (x * z + w * y);
		float r21 = 2.0f * (x * y + w * z);
		float r22 = 1.0f - 2.0f * (x * x + z * z);
		float r23 = 2.0f * (y * z - w * x);
		float r31 = 2.0f * (x * z - w * y);
		float r32 = 2.0f * (y * z + w * x);
		float r33 = 1.0f - 2.0f * (x * x + y * y);

		return mat4(
			r11, r12, r13, 0.0f,
			r21, r22, r23, 0.0f,
			r31, r32, r33, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	Quaternion operator*(const Quaternion& qa, const Quaternion& qb)
	{
		float w, x, y, z;

		w = qa.w * qb.w - qa.x * qb.x - qa.y * qb.y - qa.z * qb.z;
		x = qa.w * qb.x + qa.x * qb.w + qa.y * qb.z - qa.z * qb.y;
		y = qa.w * qb.y - qa.x * qb.z + qa.y * qb.w + qa.z * qb.x;
		z = qa.w * qb.z + qa.x * qb.y - qa.y * qb.x + qa.z * qb.w;

		return Quaternion(w, x, y, z);
	}

	void operator*=(Quaternion& qa, const Quaternion& qb)
	{
		float w, x, y, z;

		w = qa.w * qb.w - qa.x * qb.x - qa.y * qb.y - qa.z * qb.z;
		x = qa.w * qb.x + qa.x * qb.w + qa.y * qb.z - qa.z * qb.y;
		y = qa.w * qb.y - qa.x * qb.z + qa.y * qb.w + qa.z * qb.x;
		z = qa.w * qb.z + qa.x * qb.y - qa.y * qb.x + qa.z * qb.w;

		qa.w = w;
		qa.x = x;
		qa.y = y;
		qa.z = z;
	}

	std::string Quaternion::ToString()
	{
		std::stringstream ss;
		ss << "(" << w << ", <" << x << "i, " << y << "j, " << z << "k>)";
		const std::string s = ss.str();
		return s;
	}
}
