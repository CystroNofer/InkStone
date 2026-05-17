#pragma once

#include "ECS/Component.h"
#include "Math/Math.h"

namespace NXTN {
	class Transform : public BaseComponent {
	public:
		Transform();
		~Transform();

		mat4 GetMMatrix();

		vec3 position;
		vec3 scale;
		Quaternion rotation;
	};
}
