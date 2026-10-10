#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace NXTN {
	// Caution: Supports up to UINT32_MAX elements
	template<typename T>
	class SparseSet
	{
	public:
		SparseSet() = default;
		~SparseSet() = default;

		size_t Size() const noexcept {
			return m_DenseKey.size();
		}

		bool Has(uint32_t key) const {
			return static_cast<size_t>(key) < m_Sparse.size()
				&& m_Sparse[static_cast<size_t>(key)] < m_DenseKey.size()
				&& m_DenseKey[m_Sparse[static_cast<size_t>(key)]] == key;
		}

		T& Get(uint32_t key) {
			assert(Has(key) && "[Sparse Set] Item does not exist");

			return m_DenseData[m_Sparse[static_cast<size_t>(key)]];
		}

		const T& Get(uint32_t key) const {
			assert(Has(key) && "[Sparse Set] Item does not exist");

			return m_DenseData[m_Sparse[static_cast<size_t>(key)]];
		}

		bool Add(uint32_t key) {
			if (Has(key)) {
				return false;
			}
			if (static_cast<size_t>(key) >= m_Sparse.size()) {
				assert(key < UINT32_MAX && "[Sparse Set] Key exceeds UINT32_MAX - 1");
				m_Sparse.resize(static_cast<size_t>(key) + 1, SIZE_MAX);
			}
			m_Sparse[static_cast<size_t>(key)] = m_DenseKey.size();
			m_DenseKey.push_back(key);
			m_DenseData.emplace_back();

			return true;
		}

		template <typename... Args>
		bool Emplace(uint32_t key, Args&&... args) {
			if (Has(key)) {
				return false;
			}
			if (static_cast<size_t>(key) >= m_Sparse.size()) {
				assert(key < UINT32_MAX && "[Sparse Set] Key exceeds UINT32_MAX - 1");
				m_Sparse.resize(static_cast<size_t>(key) + 1, SIZE_MAX);
			}
			m_Sparse[static_cast<size_t>(key)] = m_DenseKey.size();
			m_DenseKey.push_back(key);
			m_DenseData.emplace_back(std::forward<Args>(args)...);

			return true;
		}

		void Update(uint32_t key, const T& data) {
			if (static_cast<size_t>(key) < m_Sparse.size()) {
				// Add
				if (m_Sparse[static_cast<size_t>(key)] == SIZE_MAX) {
					m_Sparse[static_cast<size_t>(key)] = m_DenseKey.size();
					m_DenseKey.push_back(key);
					m_DenseData.push_back(data);
				}
				// Update
				else {
					m_DenseData[m_Sparse[static_cast<size_t>(key)]] = data;
				}
			}
			// Add
			else {
				assert(key < UINT32_MAX && "[Sparse Set] Key exceeds UINT32_MAX - 1");
				m_Sparse.resize(static_cast<size_t>(key) + 1, SIZE_MAX);

				m_Sparse[static_cast<size_t>(key)] = m_DenseKey.size();
				m_DenseKey.push_back(key);
				m_DenseData.push_back(data);
			}
		}

		void Update(uint32_t key, T&& data) {
			if (static_cast<size_t>(key) < m_Sparse.size()) {
				// Add
				if (m_Sparse[static_cast<size_t>(key)] == SIZE_MAX) {
					m_Sparse[static_cast<size_t>(key)] = m_DenseKey.size();
					m_DenseKey.push_back(key);
					m_DenseData.push_back(std::move(data));
				}
				// Update
				else {
					m_DenseData[m_Sparse[static_cast<size_t>(key)]] = std::move(data);
				}
			}
			// Add
			else {
				m_Sparse.resize(static_cast<size_t>(key) + 1, SIZE_MAX);

				m_DenseKey.push_back(key);
				m_DenseData.push_back(std::move(data));
				m_Sparse[static_cast<size_t>(key)] = m_DenseKey.size() - 1;
			}
		}

		bool Remove(uint32_t key) {
			if (!Has(key)) {
				return false;
			}

			size_t denseIdxLast = m_DenseData.size() - 1;
			size_t denseIdxToRmv = m_Sparse[static_cast<size_t>(key)];

			if (denseIdxToRmv != denseIdxLast) {
				// The sparse for the last item in dense now maps to the one to be removed/overwritten
				m_Sparse[m_DenseKey[denseIdxLast]] = denseIdxToRmv;

				m_DenseKey[denseIdxToRmv] = m_DenseKey[denseIdxLast];
				m_DenseData[denseIdxToRmv] = std::move(m_DenseData[denseIdxLast]);
			}

			m_DenseKey.pop_back();
			m_DenseData.pop_back();

			m_Sparse[static_cast<size_t>(key)] = SIZE_MAX;

			return true;
		}

		const std::vector<uint32_t>& Keys() const {
			return m_DenseKey;
		}

	private:
		// The key type is consistent with EntityID
		std::vector<uint32_t> m_DenseKey;
		std::vector<T> m_DenseData;
		std::vector<size_t> m_Sparse;
	};
}

