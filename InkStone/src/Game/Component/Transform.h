#pragma once

#include "src/Math/Math.h"

// Angle in degrees
namespace NXTN {
	class Transform
	{
	public:
		Transform();
		~Transform() {}

		inline vec3 GetPosition() { return m_Position; }
		void Move(const vec3& dist);
		void SetPosition(const float& x, const float& y, const float& z);
		void SetPosition(const vec3& pos);

		inline vec3 GetScale() { return m_Scale; }
		void Scale(const vec3& amount);
		void SetScale(const float& x, const float& y, const float& z);
		void SetScale(const vec3& scale);

		inline Quaternion GetRotation() { return m_Rotation; }
		void Rotate(vec3 angle);
		void Rotate(float x, float y, float z);
		void RotateByAxis(float angle, const vec3& axis);
		void SetRotation(const Quaternion& rotation);

		mat4 GetModelMatrix();

	private:
		vec3 m_Position;
		vec3 m_Scale;
		Quaternion m_Rotation;
	};
}

