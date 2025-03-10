#pragma once

#include "src/Game/Component/Transform.h"

namespace NXTN {
	class Component
	{
	public:
		virtual ~Component() {}

		virtual void Update(Transform& transform) {}
	protected:
		Component() {}
	};
}