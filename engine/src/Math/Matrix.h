#pragma once

#include "Vector.h"

namespace NXTN {
	struct mat4
	{
		mat4();

		mat4(const float& a);

		mat4(
			const float& m00, const float& m01, const float& m02, const float& m03,
			const float& m10, const float& m11, const float& m12, const float& m13,
			const float& m20, const float& m21, const float& m22, const float& m23,
			const float& m30, const float& m31, const float& m32, const float& m33
		);

		inline float* operator[](int i)
		{
			return &(m_Mat[i][0]);
		}

		inline const float* operator[](int i) const
		{
			return &(m_Mat[i][0]);
		}

		//mat4 Transpose();

		std::string ToString();

	private:
		// Top-left: [0, 0], top-right: [0, 3]
		float m_Mat[4][4];
	};

	struct mat3
	{
		mat3();

		mat3(float a);

		mat3(
			const float& m00, const float& m01, const float& m02,
			const float& m10, const float& m11, const float& m12,
			const float& m20, const float& m21, const float& m22
		);

		mat3(mat4 m);

		inline float* operator[](int i)
		{
			return &(m_Mat[i][0]);
		}

		inline const float* operator[](int i) const
		{
			return &(m_Mat[i][0]);
		}

		//mat3 Transpose();

		std::string ToString();

	private:
		// Top-left: [0, 0], top-right: [0, 3]
		float m_Mat[3][3];
	};

	mat3 Transpose(const mat3& m);
	mat4 Transpose(const mat4& m);

	mat4 TranslateMat(vec3 translation);
	mat4 TranslateMat(mat4 m, vec3 translation);
	mat4 TranslateMat(vec4 translation);
	mat4 TranslateMat(mat4 m, vec4 translation);
	mat4 RotateMat(vec3 angles);
	mat4 ScaleMat(vec3 scales);

	vec4 mul(mat4 mat, vec4 vec);
	mat4 mul(mat4 m1, mat4 m2);

	std::ostream& operator<<(std::ostream& ostr, mat3& m);
	std::ostream& operator<<(std::ostream& ostr, mat4& m);
}