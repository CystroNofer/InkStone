#pragma once

#include "Container/SparseSet.h"
#include "Scene.h"  // Contains E and C

namespace NXTN {
	// Callback constraint for Each()
	template <typename F, typename... Cs>
	concept IsComponentCallback =
		(IsComponent<Cs> && ...) && std::invocable<F&, EntityID, Cs&...>;

	// Ensure not duplicated component types in combination
	template <typename...>
	struct UniqueComponent : std::true_type {};

	template <typename C, typename... Rest>
	struct UniqueComponent<C, Rest...> : std::bool_constant<((!std::is_same_v<C, Rest>) && ...) && UniqueComponent<Rest...>::value> {};

	// Sparse set wrappers
	class IComponentStorage {
	public:
		virtual ~IComponentStorage() {}
		virtual size_t Size() const noexcept = 0;
		virtual bool Has(EntityID eid) const = 0;
		virtual void Add(EntityID eid) = 0;
		virtual void Remove(EntityID eid) = 0;
		virtual const std::vector<uint32_t>& Keys() const = 0;

	private:
	};

	template <typename C>
		requires (IsComponent<C>)
	class ComponentStorage final : public IComponentStorage {
	public:
		virtual size_t Size() const noexcept override {
			return m_Set.Size();
		}

		virtual bool Has(EntityID eid) const override {
			return m_Set.Has(eid);
		}

		C& Get(EntityID eid) {
			return m_Set.Get(eid);
		}

		virtual void Add(EntityID eid) override {
			m_Set.Add(eid);
		}

		template <typename... Args>
		void Emplace(EntityID eid, Args&&... args) {
			m_Set.Emplace(eid, std::forward<Args>(args)...);
		}

		virtual void Remove(EntityID eid) override {

		}

		virtual const std::vector<uint32_t>& Keys() const override {
			return m_Set.Keys();
		}

	private:
		SparseSet<C> m_Set;
	};

	// Non-singleton design for future proof
	// Static destruction can potentially cause bugs
	class Registry
	{
	public:
		Registry();
		~Registry();

		EntityID NewEntity();

		void DestroyEntity(Entity& e);

		template <typename C>
			requires (IsComponent<C>)
		void AddComponent(EntityID eid) {
			ComponentID cid = ComponentIDOf<C>();

			if (cid >= m_Components.size()) {
				m_Components.resize(cid + 1);
				m_Components[cid].reset(new ComponentStorage<C>);
			}
			else if (m_Components[cid] == nullptr) {
				m_Components[cid].reset(new ComponentStorage<C>);
			}
			m_Components[cid]->Add(eid);
		}

		template <typename C, typename... Args>
			requires (IsComponent<C>)
		void AddComponent(EntityID eid, Args&&... args) {
			ComponentID cid = ComponentIDOf<C>();

			if (cid >= m_Components.size()) {
				m_Components.resize(cid + 1);
				m_Components[cid].reset(new ComponentStorage<C>);
			}
			else if (m_Components[cid] == nullptr) {
				m_Components[cid].reset(new ComponentStorage<C>);
			}
			static_cast<ComponentStorage<C>*>(m_Components[cid].get())->Emplace(eid, std::forward<Args>(args)...);
		}

		template <typename... Cs, typename F>
			requires (IsComponentCallback<F, Cs...> && UniqueComponent<Cs...>::value)
		void Each(F&& callback) {
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
					//// Missing is expected, consider skip warnings
					//std::string names;
					//((names += typeid(Cs).name(), names += ' '), ...);
					//Log::Warning("No entity has component(s) of type(s) %s\n", names.c_str());
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

