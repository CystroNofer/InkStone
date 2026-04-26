#pragma once

#include "Registry.h"

namespace NXTN {
	class BaseECSSystem {
	public:
		virtual void Run(Registry& r) = 0;

	protected:
		virtual ~BaseECSSystem() = default;
	};
}
