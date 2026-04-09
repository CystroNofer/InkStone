#pragma once

#include "src/ECS/Component.h"

namespace NXTN {
	class Transform : public BaseComponent {
	public:
		Transform();
		~Transform();

		mat4 GetMMatrix();

		vec3 position;
		vec3 scale;
		vec4 rotation;
	};
}
