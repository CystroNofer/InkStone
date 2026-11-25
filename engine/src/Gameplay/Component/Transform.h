#pragma once

#include "ECS/Component.h"
#include "Math/Math.h"

namespace NXTN {
	class Transform : public ECSComponent {
	public:
		Transform();
		~Transform();

		vec3 Forward() const;
		vec3 Backward() const;
		vec3 Right() const;
		vec3 Left() const;
		vec3 Up() const;
		vec3 Down() const;

		void Rotate(const Quaternion& deltaRotation, FrameOfReference frame = FrameOfReference::Local);

		mat4 GetMMatrix() const;

		// Local to the parent
		vec3 position;
		vec3 scale;
		Quaternion rotation;
	};
}
