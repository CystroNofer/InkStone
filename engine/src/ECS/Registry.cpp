#include "pch.h"

#include "Registry.h"
#include "Log.h"

namespace NXTN {
	Registry::Registry() {

	}

	Registry::~Registry() {

	}

	Entity Registry::NewEntity(std::string name) {
		EntityID i;
		// No recycled ID
		if (m_RecycledEntityIDs.empty()) {
			i = static_cast<EntityID>(m_Generations.size());
			m_Generations.push_back(0);
		}
		// Reuse ID
		else
		{
			EntityID i = m_RecycledEntityIDs.back();
			m_RecycledEntityIDs.pop_back();
			// Generation already incremented when destroyed
		}
		Entity e{ i, 0 };

		Log::Info("New entity: ID %d, Gen %d", i, m_Generations[i]);

		AddComponent<NameTag>(e, name);

		return e;
	}

	void Registry::DestroyEntity(Entity& e) {
		if (!IsAlive(e)) {
			Log::Warning("Stale entity %d, gen %d\n", e.id, e.generation);
			return;
		}
		for (std::unique_ptr<IComponentStorage>& storagePtr : m_ComponentStorages) {
			storagePtr->Remove(e.id);
		}
		m_RecycledEntityIDs.push_back(e.id);
		m_Generations[e.id]++;

		Log::Info("Destoryed entity: ID %d, Gen %d", e.id, e.generation);
	}
}
