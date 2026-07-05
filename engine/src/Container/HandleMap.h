#pragma once

#include <cmath>
#include <vector>
#include <memory>

namespace NXTN {
	template <typename T>
	struct Handle {
	public:
		size_t id = SIZE_MAX;
		uint32_t gen = 0u;

		static const Handle invalid;

		bool operator==(const Handle<T>& h) const {
			return h.id == id && h.gen == gen;
		}
	};

	template <typename T>
	inline constexpr Handle<T> Handle<T>::invalid = { SIZE_MAX, 0 };

	template <typename T>
	class HandleMap {
	public:
		HandleMap() {}

		Handle<T> Add(T* itemPtr) {
			for (size_t i = 0; i < m_Map.size(); i++) {
				if (m_Map[i].second == nullptr) {
					m_Map[i].second.reset(itemPtr);
					// Generation is incremented when the last was destroyed
					return { i, m_Map[i].first };
				}
			}
			m_Map.push_back({ 1, std::unique_ptr<T>(itemPtr) });
			return { m_Map.size() - 1, 1 };
		}

		bool Validate(Handle<T> handle) {
			return handle.id < m_Map.size() && m_Map[handle.id].first == handle.gen;
		}

		T* Get(Handle<T> handle) {
			if (Validate(handle)) {
				return m_Map[handle.id].second.get();
			}

			return nullptr;
		}

		void Remove(Handle<T> handle)
		{
			if (Validate(handle)) {
				m_Map[handle.id].second.reset();
				m_Map[handle.id].first++;
			}
		}

	private:
		// Get() has to return T*
		// If the vector stores T then relocate when scaling
		// T* might become invalid
		std::vector<std::pair<uint32_t, std::unique_ptr<T>>> m_Map;
	};
}