#pragma once

#include "ECS/Component.h"

#include <string>

namespace NXTN {
	class NameTag : public BaseComponent {
	public:
		NameTag() = default;
		NameTag(const std::string& name) : name(name) {}
		~NameTag() = default;

		std::string name;
	};
}
