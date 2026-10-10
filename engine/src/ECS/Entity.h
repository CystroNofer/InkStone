#pragma once

#include <cstdint>

namespace NXTN {
	typedef uint32_t EntityID;
	typedef uint32_t EntityGeneration;

	class Entity
	{
	public:
		Entity() = delete;

		Entity(EntityID id, EntityGeneration gen)
			: id(id), generation(gen)
		{ }

		const EntityID id;
		const EntityGeneration generation;

		bool operator==(const Entity& e) const {
			return e.id == id && e.generation == generation;
		}
	};
}
