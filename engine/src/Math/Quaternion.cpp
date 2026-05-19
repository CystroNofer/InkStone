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

	Quaternion Quaternion::FromRotationMatrix(mat4 r)
	{
		// The [] operator is not trivial
		float r00 = r[0][0];
		float r01 = r[0][1];
		float r02 = r[0][2];
		float r10 = r[1][0];
		float r11 = r[1][1];
		float r12 = r[1][2];
		float r20 = r[2][0];
		float r21 = r[2][1];
		float r22 = r[2][2];

		float trace = r00 + r11 + r22;

		if (trace > EPSILON) {
			float s = 2.0f * std::sqrt(trace + 1);
			return Quaternion(0.25f * s, (r21 - r12) / s, (r02 - r20) / s, (r10 - r01) / s);
		}
		if (r00 > r11 && r00 > r22) {
			float s = 2.0f * std::sqrt(1.0f + r00 - r11 - r22);
			return Quaternion((r21 - r12) / s, 0.25f * s, (r01 + r10) / s, (r02 + r20) / s);
		}
		else if (r11 > r22)
		{
			float s = 2.0f * std::sqrt(1.0f + r11 - r00 - r22);
			return Quaternion((r02 - r20) / s, (r01 + r10) / s, 0.25f * s, (r12 + r21) / s);
		}
		else
		{
			float s = 2.0f * std::sqrt(1.0f + r22 - r00 - r11);
			return Quaternion((r10 - r01) / s, (r02 + r20) / s, (r12 + r21) / s, 0.25f * s);
		}
	}

	void normalize(Quaternion& q) {
		float l = q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z;

		if (l < EPSILON_SQ) {
			q.w = 1.0f;
			q.x = 0.0f;
			q.y = 0.0f;
			q.z = 0.0f;
			return;
		}
		
		float invSqrtL = 1.0f / std::sqrt(l);

		q.w *= invSqrtL;
		q.x *= invSqrtL;
		q.y *= invSqrtL;
		q.z *= invSqrtL;
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
