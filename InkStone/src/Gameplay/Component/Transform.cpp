#include "pch.h"

#include "Transform.h"

namespace NXTN {
	Transform::Transform() : scale(1.0f), rotation(0.0f, 0.0f, 0.0f, 1.0f) {}

	Transform::~Transform() {}

	mat4 Transform::GetMMatrix()
	{
		float r11 = (1.0f - 2.0f * (rotation.y * rotation.y + rotation.z * rotation.z)) * scale.x;
		float r12 = 2.0f * (rotation.x * rotation.y - rotation.w * rotation.z) * scale.y;
		float r13 = 2.0f * (rotation.x * rotation.z + rotation.w * rotation.y) * scale.z;
		float r21 = 2.0f * (rotation.x * rotation.y + rotation.w * rotation.z) * scale.x;
		float r22 = (1.0f - 2.0f * (rotation.x * rotation.x + rotation.z * rotation.z)) * scale.y;
		float r23 = 2.0f * (rotation.y * rotation.z - rotation.w * rotation.x) * scale.z;
		float r31 = 2.0f * (rotation.x * rotation.z - rotation.w * rotation.y) * scale.x;
		float r32 = 2.0f * (rotation.y * rotation.z + rotation.w * rotation.x) * scale.y;
		float r33 = (1.0f - 2.0f * (rotation.x * rotation.x + rotation.y * rotation.y)) * scale.z;

		return mat4(
			r11, r12, r13, position.x,
			r21, r22, r23, position.y,
			r31, r32, r33, position.z,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}
}