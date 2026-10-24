#pragma once

#include <optional>
#include <span>
#include <string_view>
#include <tuple>
#include <vector>

#include "Container/SparseSet.h"
#include "Entity.h"
#include "Component.h"
#include "Gameplay/Component/NameTag.h"
#include "Gameplay/Component/Transform.h"

#include "Log.h"


namespace NXTN {
	class Camera;
	class Scene;

	// Borrowed read-only access. The scene must outlive the view.
	// Do not change scene structure or names while consuming returned spans/string views.
	class SceneHierarchyView {
	public:
		std::vector<Entity> GetRoots() const;
		std::span<const Entity> GetChildren(Entity entity) const;
		std::string_view GetName(Entity entity) const;
		// Opaque identity for scoping UI state; valid only during the scene's lifetime.
		const void* GetIdentity() const;

	private:
		friend class Scene;
		explicit SceneHierarchyView(const Scene& scene);
		const Scene* m_Scene;
	};

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

		const C& Get(EntityID id) const {
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

	// Non-singleton
	// Static destruction can potentially cause bugs
	class Scene
	{
	public:
		Scene(std::string name = "Unnamed Scene");
		~Scene();

		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;

		void Render(Camera& camera, const Transform& cameraTransform);

		// Every entity starts parentless with NameTag; Transform is opt-in.
		// Use NewEntity<Transform>() or AddComponent<Transform>() for spatial entities.
		// Structural changes are prohibited inside Each(); NewEntity throws in that case.
		Entity NewEntity(std::string name = "Unnamed Object");

		template <typename... Ts>
			requires ((IsComponent<Ts> && ...) && (std::default_initializable<Ts> && ...))
		Entity NewEntity(std::string name = "Unnamed Object") {
			Entity entity = NewEntity(name);
			try {
				(AddInitialComponent<Ts>(entity), ...);
			}
			catch (...) {
				DestroyEntity(entity);
				throw;
			}
			return entity;
		}

		inline bool IsAlive(Entity e) const {
			return e.GetID() < m_Generations.size() && m_Generations[e.GetID()] == e.GetGeneration()
				&& m_Hierarchy[e.GetID()].has_value();
		}

		// Handles are scene-local. Cross-scene identity checks are not implemented
		// Keep-local parenting: local TRS stays unchanged
		// Parenting to std::nullopt makes a root
		// Requires a Transform on the child and on any supplied parent.
		bool SetParent(Entity child, std::optional<Entity> parent);

		std::optional<Entity> GetParent(Entity entity) const;
		// Borrowed views
		// Avoid retaining or mutating the returned span
		std::span<const Entity> GetChildren(Entity entity) const;
		// Snapshot of live parentless entities with Transform, ordered by ID; handles can become stale.
		std::vector<Entity> GetRoots() const;
		SceneHierarchyView GetHierarchyView() const &;
		SceneHierarchyView GetHierarchyView() const && = delete;

		// Removes the whole subtree
		// Returns false for stale handles or during Each().
		bool DestroyEntity(Entity entity);

		// World matrices are snapshots
		// Recompute after local edits
		// Render calls this automatically
		void UpdateWorldTransforms();
		// Returns nullopt for stale entities or entities without Transform.
		std::optional<mat4> GetWorldMatrix(Entity entity) const;

		template <typename C> requires (IsComponent<C>)
		C* TryGetComponent(Entity entity) {
			if (!IsAlive(entity)) { return nullptr; }
			ComponentStorage<C>* storage = TryGetStorage<C>();
			return storage && storage->Has(entity.GetID()) ? &storage->Get(entity.GetID()) : nullptr;
		}

		template <typename C> requires (IsComponent<C>)
		const C* TryGetComponent(Entity entity) const {
			if (!IsAlive(entity)) { return nullptr; }
			const ComponentStorage<C>* storage = TryGetStorage<C>();
			return storage && storage->Has(entity.GetID()) ? &storage->Get(entity.GetID()) : nullptr;
		}

		template <typename C>
			requires (IsComponent<C>)
		bool AddComponent(Entity e) {
			if (m_IterationDepth != 0) { return false; }
			if (!IsAlive(e)) {
				Log::Warning("Stale entity %u, gen %u\n", e.GetID(), e.GetGeneration());
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
			if (C::UNIQUE_PER_ENTITY && storage->Has(e.GetID())) {
				Log::Warning("Duplicate component of type %s on entity %u\n", typeid(C).name(), e.GetID());
				return false;
			}
			storage->Add(e.GetID());
			return true;
		}

		template <typename C, typename... Args>
			requires (IsComponent<C>)
		bool AddComponent(Entity e, Args&&... args) {
			if (m_IterationDepth != 0) { return false; }
			if (!IsAlive(e)) {
				Log::Warning("Stale entity %u, gen %u\n", e.GetID(), e.GetGeneration());
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
			if (C::UNIQUE_PER_ENTITY && storage->Has(e.GetID())) {
				Log::Warning("Duplicate component of type %s on entity %u\n", typeid(C).name(), e.GetID());
				return false;
			}
			storage->Emplace(e.GetID(), std::forward<Args>(args)...);
			return true;
		}

		template <typename... Cs, typename F>
			requires (IsComponentCallback<F, Cs...>&& UniqueComponent<Cs...>::value)
		void Each(F&& callback) {
			IterationGuard guard(m_IterationDepth);
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
		struct HierarchyNode {
			std::optional<Entity> parent;
			std::vector<Entity> children;
			mat4 worldMatrix = mat4(1.0f);
		};

		struct IterationGuard {
			explicit IterationGuard(size_t& depth) : depth(depth) { depth++; }
			~IterationGuard() { depth--; }
			IterationGuard(const IterationGuard&) = delete;
			IterationGuard& operator=(const IterationGuard&) = delete;
			size_t& depth;
		};

		template <typename C>
		void AddInitialComponent(Entity entity) {
			// NameTag is already supplied by the non-template NewEntity overload.
			if constexpr (!std::same_as<C, NameTag>) {
				AddComponent<C>(entity);
			}
		}

		template <typename C> requires (IsComponent<C>)
		const ComponentStorage<C>* TryGetStorage() const {
			ComponentID cid = ComponentIDOf<C>();
			if (cid >= m_ComponentStorages.size()) { return nullptr; }
			return static_cast<const ComponentStorage<C>*>(m_ComponentStorages[cid].get());
		}

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
		// An empty slot is a destroyed entity; links never own components or resources.
		std::vector<std::optional<HierarchyNode>> m_Hierarchy;
		size_t m_IterationDepth = 0;

		// Component storage
		std::vector<std::unique_ptr<IComponentStorage>> m_ComponentStorages;
	};
}
