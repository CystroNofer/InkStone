#pragma once

#include <optional>
#include <tuple>
#include <vector>

#include "Container/SparseSet.h"
#include "Entity.h"
#include "Component.h"
#include "Gameplay/Component/NameTag.h"

#include "Log.h"


namespace NXTN {
	class Camera;
	class Transform;
	struct mat4;

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
		virtual bool Has(EntityID id) const = 0;
		virtual void Add(EntityID id) = 0;
		virtual void Remove(EntityID id) = 0;
		virtual const std::vector<uint32_t>& Keys() const = 0;
	};

	template <typename C>
		requires (IsComponent<C>)
	class ComponentStorage final : public IComponentStorage {
	public:
		virtual size_t Size() const noexcept override {
			return m_Set.Size();
		}

		virtual bool Has(EntityID id) const override {
			return m_Set.Has(id);
		}

		C& Get(EntityID id) {
			return m_Set.Get(id);
		}

		virtual void Add(EntityID id) override {
			m_Set.Add(id);
		}

		template <typename... Args>
		void Emplace(EntityID id, Args&&... args) {
			m_Set.Emplace(id, std::forward<Args>(args)...);
		}

		virtual void Remove(EntityID id) override {
			m_Set.Remove(id);
		}

		virtual const std::vector<uint32_t>& Keys() const override {
			return m_Set.Keys();
		}

	private:
		SparseSet<C> m_Set;
	};

	// Non-singleton design for future proof
	// Static destruction can potentially cause bugs
	class Scene
	{
	public:
		Scene(std::string name = "Unnamed Scene");
		~Scene();

		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;

		void Render(Camera& camera, const Transform& cameraTransform);

		Entity NewEntity(std::string name = "Unnamed Object");

		template <typename... Ts>
			requires ((IsComponent<Ts> && ...) && (std::default_initializable<Ts> && ...))
		Entity NewEntity(std::string name = "Unnamed Object") {
			Entity entity = NewEntity(name);
			(AddComponent<Ts>(entity), ...);
			return entity;
		}

		inline bool IsAlive(Entity e) const {
			return e.id < m_Generations.size() && m_Generations[e.id] == e.generation;
		}

		void DestroyEntity(Entity& e);

		template <typename C>
			requires (IsComponent<C>)
		bool AddComponent(Entity e) {
			if (!IsAlive(e)) {
				Log::Warning("Stale entity %d, gen %d\n", e.id, e.generation);
				return false;
			}
			ComponentID cid = ComponentIDOf<C>();

			if (cid >= m_ComponentStorages.size()) {
				m_ComponentStorages.resize(cid + 1);
				m_ComponentStorages[cid].reset(new ComponentStorage<C>);
			}
			else if (m_ComponentStorages[cid] == nullptr) {
				m_ComponentStorages[cid].reset(new ComponentStorage<C>);
			}
			ComponentStorage<C>* storage = static_cast<ComponentStorage<C>*>(m_ComponentStorages[cid].get());
			if (C::UNIQUE_PER_ENTITY && storage->Has(e.id)) {
				Log::Warning("Duplicate component of type %s on entity %d\n", typeid(C).name(), e.id);
				return false;
			}
			storage->Add(e.id);
			return true;
		}

		template <typename C, typename... Args>
			requires (IsComponent<C>)
		bool AddComponent(Entity e, Args&&... args) {
			if (!IsAlive(e)) {
				Log::Warning("Stale entity %d, gen %d\n", e.id, e.generation);
				return false;
			}
			ComponentID cid = ComponentIDOf<C>();

			if (cid >= m_ComponentStorages.size()) {
				m_ComponentStorages.resize(cid + 1);
				m_ComponentStorages[cid].reset(new ComponentStorage<C>);
			}
			else if (m_ComponentStorages[cid] == nullptr) {
				m_ComponentStorages[cid].reset(new ComponentStorage<C>);
			}
			ComponentStorage<C>* storage = static_cast<ComponentStorage<C>*>(m_ComponentStorages[cid].get());
			if (C::UNIQUE_PER_ENTITY && storage->Has(e.id)) {
				Log::Warning("Duplicate component of type %s on entity %d\n", typeid(C).name(), e.id);
				return false;
			}
			storage->Emplace(e.id, std::forward<Args>(args)...);
			return true;
		}

		template <typename... Cs, typename F>
			requires (IsComponentCallback<F, Cs...>&& UniqueComponent<Cs...>::value)
		void Each(F&& callback) {
			// Retrieve storages
			constexpr size_t kCs = sizeof...(Cs);

			std::tuple<ComponentStorage<Cs>*...> compStorages = { TryGetStorage<Cs>()... };
			std::vector<IComponentStorage*> iCompStorages = {
				static_cast<IComponentStorage*>(std::get<ComponentStorage<Cs>*>(compStorages))...
			};

			// Find driver (smallest storage)
			size_t driverStorageSize = SIZE_MAX;
			size_t driverStorageIdx = 0;
			for (size_t i = 0; i < kCs; i++) {
				if (iCompStorages[i] == nullptr) {
					// Missing is expected
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

			if (cid >= m_ComponentStorages.size()) {
				return nullptr;
			}
			return static_cast<ComponentStorage<C>*>(m_ComponentStorages[cid].get());
		}

		template <typename C>
			requires (IsComponent<C>)
		ComponentStorage<C>* GetOrCreateStorage() {
			ComponentID cid = ComponentIDOf<C>();

			if (cid >= m_ComponentStorages.size()) {
				m_ComponentStorages.resize(cid + 1);
				m_ComponentStorages[cid].reset(new ComponentStorage<C>);
			}
			else if (m_ComponentStorages[cid] == nullptr) {
				m_ComponentStorages[cid].reset(new ComponentStorage<C>);
			}
			return static_cast<ComponentStorage<C>*>(m_ComponentStorages[cid].get());
		}

		static mat4 GetVMatrix(const Transform& cameraTransform);

		std::optional<Entity> m_MainCamera;
		std::string m_Name;

		// For entity ID
		std::vector<EntityID> m_RecycledEntityIDs;
		std::vector<EntityGeneration> m_Generations;

		// Component storage
		std::vector<std::unique_ptr<IComponentStorage>> m_ComponentStorages;
	};
}
