#include "pch.h"

#include "Registry.h"
#include "src/Log.h"

namespace NXTN {
	Registry::Registry() {

	}

	Registry::~Registry() {

	}

	EntityID Registry::NewEntity() {
		// No recycled ID
		if (m_RecycledEntityIDs.empty()) {
			size_t i = m_Generations.size();
			if (i > UINT32_MAX) {
				Log::Error("Entity count exceeds UINT32_MAX");
			}
			m_Generations.push_back(0);

			Log::Info("New entity: ID %d, Gen 0", i);

			return static_cast<EntityID>(i);
		}
		// Reuse ID
		EntityID i = m_RecycledEntityIDs.back();
		m_RecycledEntityIDs.pop_back();
		// Generation incremented when destroyed

		Log::Info("New entity: ID %d, Gen %d", i, m_Generations[i]);

		return static_cast<EntityID>(i);
	}

	void Registry::DestroyEntity(Entity& e) {
		if (e.id >= m_Generations.size()) {
			Log::Error("Invalid entity ID");
		}
		// Stale entity
		if (e.generation < m_Generations[e.id]) {
			return;
		}
		for (std::unique_ptr<IComponentStorage>& storagePtr : m_Components) {
			storagePtr->Remove(e.id);
		}
		m_RecycledEntityIDs.push_back(e.id);
		m_Generations[e.id]++;

		Log::Info("Destoryed entity: ID %d, Gen %d", e.id, e.generation);
	}
}
