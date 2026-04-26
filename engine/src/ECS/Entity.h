#pragma once

#include <cstdint>

namespace NXTN {
	typedef uint32_t EntityID;
	typedef uint32_t GenerationID;

	class Entity
	{
	public:
		inline Entity(EntityID id, GenerationID gen)
			: id(id), generation(gen)
		{ }

		const EntityID id;
		const GenerationID generation;
	};
}

