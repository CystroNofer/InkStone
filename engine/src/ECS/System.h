#pragma once

#include "Scene.h"

namespace NXTN {
	class BaseECSSystem {
	public:
		virtual void Run(Scene& scene) = 0;

	protected:
		virtual ~BaseECSSystem() = default;
	};
}
