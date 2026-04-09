#pragma once

#include <cassert>
#include <concepts>

#include "src/Math/Math.h"
#include "Entity.h"

namespace NXTN {
	// Types and constants
	//typedef size_t ComponentID;
	typedef uint32_t ComponentID; // Platform-independent
	constexpr ComponentID COMPONENT_ID_MAX = UINT32_MAX;

	class BaseComponent {
	protected:
		virtual ~BaseComponent() = default;
	};

	template <typename C>
	concept IsComponent = std::derived_from<
		std::remove_cvref_t<C>,
		BaseComponent
	>;

	// For component to type ID mapping
	namespace detail {
		inline ComponentID AllocateComponentID() {
			static ComponentID s_NextCID = 0;
			assert(s_NextCID < COMPONENT_ID_MAX && "COMPONENT_ID_MAX exceeded");
			return ++s_NextCID;
		}
	}

	template <typename C>
		requires (IsComponent<C>)
	ComponentID ComponentIDOf() {
		static ComponentID cid = detail::AllocateComponentID();
		return cid;
	}
}