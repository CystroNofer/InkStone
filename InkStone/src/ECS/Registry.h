#pragma once

#include "Scene.h"

namespace NXTN {
	// Callback constraint for Each()
	template <typename F, typename... Cs>
	concept IsComponentCallback =
		(IsComponent<Cs> && ...) && std::invocable<F&, EntityID&, Cs&...>;

	// Ensure not duplicated component types in combination
	template <typename...>
	struct UniqueComponent : std::true_type {};

	template <typename C, typename... Rest>
	struct UniqueComponent<C, Rest...> : std::bool_constant<((!std::is_same_v<C, Rest>) && ...) && UniqueComponent<Rest...>::value> {};

	// Non-singleton design for future proof
	// Static destruction can potentially cause bugs
	class Registry
	{
		Registry();
		~Registry();

		EntityID NewEntity();
		void DestroyEntity(Entity& e);

		template <typename C>
			requires (IsComponent<C>)
		void AddComponent(Entity& e) {
			ComponentID cid = ComponentIDOf<C>();

			if (cid >= m_Components.size()) {
				m_Components.resize(cid + 1);
				m_Components[cid].reset(new ComponentStorage<C>);
			}
			else if (m_Components[cid] == nullptr) {
				m_Components[cid].reset(new ComponentStorage<C>);
			}
			m_Components[cid]->Add(e.id);
		}

		template <typename F, typename... Cs>
			requires (IsComponentCallback<F, Cs...> && UniqueComponent<Cs...>::value)
		void Each(F callback) {
			// Retrieve storages
			constexpr size_t kCs = sizeof...(Cs);

			std::tuple<ComponentStorage<Cs>*...> compStorages = { TryGetStorage<Cs>()... };
			std::array<IComponentStorage*, kCs> iCompStorages = {
				static_cast<IComponentStorage*>(std::get<ComponentStorage<Cs>*>(compStorages))...
			};

			// Find driver (smallest storage)
			size_t driverStorageSize = SIZE_MAX;
			size_t driverStorageIdx = 0;
			for (size_t i = 0; i < kCs; i++) {
				if (iCompStorages[i] == nullptr) {
					// Missing is expected, consider skip warnings
					std::string names;
					((names += typeid(Cs).name(), names += ' '), ...);
					Log::Warning("No entity has component(s) of type(s) %s\n", names.c_str());
					return;
				}
				if (iCompStorages[i]->Size() < driverStorageSize) {
					driverStorageSize = iCompStorages[i]->Size();
					driverStorageIdx = i;
				}
			}

			for (EntityID k : iCompStorages[driverStorageIdx]->Keys()) {
				bool has = true;
				for (size_t i = 0; i < kCs; i++) {
					has &= i == driverStorageIdx || iCompStorages[i]->Has(k);
				}

				if (has) {
					callback(k, std::get<ComponentStorage<Cs>*>(compStorages)->Get(k)...);
				}
			}
		}

	private:
		template <typename C>
			requires (IsComponent<C>)
		ComponentStorage<C>* TryGetStorage() {
			ComponentID cid = ComponentIDOf<C>();

			if (cid >= m_Components.size()) {
				return nullptr;
			}
			return static_cast<ComponentStorage<C>*>(m_Components[cid].get());
		}

		template <typename C>
			requires (IsComponent<C>)
		ComponentStorage<C>* GetOrCreateStorage() {
			ComponentID cid = ComponentIDOf<C>();

			if (cid >= m_Components.size()) {
				m_Components.resize(cid + 1);
				m_Components[cid].reset(new ComponentStorage<C>);
			}
			else if (m_Components[cid] == nullptr) {
				m_Components[cid].reset(new ComponentStorage<C>);
			}
			return static_cast<ComponentStorage<C>*>(m_Components[cid].get());
		}

		// For entity ID
		std::vector<EntityID> m_RecycledEntityIDs;
		std::vector<GenerationID> m_Generations;

		// Component storage
		std::vector<std::unique_ptr<IComponentStorage>> m_Components;
	};
}

