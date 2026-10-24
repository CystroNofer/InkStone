#pragma once

#include "ECS/Component.h"
#include "Math/Math.h"

namespace NXTN {
	class Transform : public BaseComponent {
	public:
		Transform();
		~Transform();

		mat4 GetMMatrix() const;

		// Local to the parent; roots are relative to world space.
		vec3 position;
		vec3 scale;
		Quaternion rotation;
	};
}
