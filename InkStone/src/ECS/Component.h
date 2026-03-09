#pragma once

#include <concepts>

#include "src/Container/SparseSet.h"
#include "Entity.h"
#include "src/Log.h"

namespace NXTN {
	// Types and constants
	//typedef size_t ComponentID;
	typedef uint32_t ComponentID; // Platform-independent
	constexpr ComponentID COMPONENT_ID_MAX = UINT32_MAX;

	class BaseComponent {
		friend ComponentID ComponentIDOf();

	protected:
		virtual ~BaseComponent() = default;

	private:
		// For component to type ID mapping
		static inline ComponentID AllocateComponentID() {
			static ComponentID s_NextCID = 0;
			if (s_NextCID < COMPONENT_ID_MAX) return ++s_NextCID;
			Log::Warning("COMPONENT_ID_MAX exceeded");
		}
	};

	template <typename C>
	concept IsComponent = std::derived_from<
		std::remove_cvref_t<C>,
		BaseComponent
	>;

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
	class ComponentStorage final : IComponentStorage {
	public:
		virtual size_t Size() const noexcept override {
			return m_Set.Size();
		}

		virtual bool Has(EntityID eid) const override {
			return m_Set.Has(eid);
		}

		C& Get(EntityID eid) {
			return m_Set->Get(eid);
		}

		virtual void Add(EntityID eid) override {
			m_Set.Add(eid);
		}

		virtual void Remove(EntityID eid) override {

		}

		virtual const std::vector<uint32_t>& Keys() const override {
			return m_Set.Keys();
		}

	private:
		SparseSet<C> m_Set;
	};

	template <typename C>
		requires (IsComponent<C>)
	ComponentID ComponentIDOf() {
		static ComponentID cid = BaseComponent::AllocateComponentID();
		return cid;
	}
}