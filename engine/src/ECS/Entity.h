#pragma once

#include <cstdint>

namespace NXTN {
	typedef uint32_t EntityID;
	typedef uint32_t EntityGeneration;
	class Scene;

	class Entity
	{
	public:
		Entity() = delete;

		constexpr EntityID GetID() const noexcept { return m_ID; }
		constexpr EntityGeneration GetGeneration() const noexcept { return m_Generation; }

		constexpr bool operator==(const Entity& e) const noexcept {
			return e.m_ID == m_ID && e.m_Generation == m_Generation;
		}

	private:
		friend class Scene;
		constexpr Entity(EntityID id, EntityGeneration generation) noexcept
			: m_ID(id), m_Generation(generation) {}

		EntityID m_ID;
		EntityGeneration m_Generation;
	};
}
