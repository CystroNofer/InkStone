#pragma once

#include "Component.h"

namespace NXTN {
	typedef uint32_t ComponentID; // Platform-independent
	//typedef size_t ComponentID;

	class Scene
	{
	public:
		Scene();
		~Scene();

		EntityID NewEntity();
		void DestroyEntity(Entity& e);

	private:
		// For component to type ID mapping
		ComponentID AllocateComponentID();
		// It is necessary to hide the counter in a .cpp file
		// To ensure no other translation unit could define duplicates
		// Thus, AllocateComponentID is used to avoid mentioning it in .h

		template <typename T>
		ComponentID ComponentIDOf() {
			static ComponentID cid = AllocateComponentID();
			return cid;
		}

		// For entity ID
		std::vector<EntityID> m_RecycledEntityIDs;
		std::vector<GenerationID> m_Generations;

		// Component storage
		std::vector<std::unique_ptr<IComponentStorage>> m_Components;
	};
}
