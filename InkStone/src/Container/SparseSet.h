#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace NXTN {
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
			return key < m_Sparse.size()
				&& m_Sparse[key] < m_DenseKey.size()
				&& m_DenseKey[m_Sparse[key]] == key;
		}

		T& Get(uint32_t key) {
			assert(Has(key) && "[Sparse Set] Item does not exist");

			return m_DenseData[m_Sparse[key]];
		}

		const T& Get(uint32_t key) const {
			assert(Has(key) && "[Sparse Set] Item does not exist");

			return m_DenseData[m_Sparse[key]];
		}

		bool Add(uint32_t key) {

			if (Has(key)) {
				return false;
			}
			if (key >= m_Sparse.size()) {
				m_Sparse.resize(static_cast<size_t>(key) + 1, SIZE_MAX);
			}
			m_Sparse[key] = m_DenseKey.size();
			m_DenseKey.push_back(key);
			m_DenseData.emplace_back();

			return true;
		}

		void Update(uint32_t key, const T& data) {
			if (key < m_Sparse.size()) {
				// Add
				if (m_Sparse[key] == SIZE_MAX) {
					m_DenseKey.push_back(key);
					m_DenseData.push_back(data);
					m_Sparse[key] = m_DenseKey.size() - 1;
				}
				// Update
				else {
					m_DenseData[m_Sparse[key]] = data;
				}
			}
			// Add
			else {
				m_Sparse.resize(static_cast<size_t>(key) + 1, SIZE_MAX);

				m_DenseKey.push_back(key);
				m_DenseData.push_back(data);
				m_Sparse[key] = m_DenseKey.size() - 1;
			}
		}

		void Update(uint32_t key, T&& data) {
			if (key < m_Sparse.size()) {
				// Add
				if (m_Sparse[key] == SIZE_MAX) {
					m_DenseKey.push_back(key);
					m_DenseData.push_back(std::move(data));
					m_Sparse[key] = m_DenseKey.size() - 1;
				}
				// Update
				else {
					m_DenseData[m_Sparse[key]] = std::move(data);
				}
			}
			// Add
			else {
				m_Sparse.resize(static_cast<size_t>(key) + 1, SIZE_MAX);

				m_DenseKey.push_back(key);
				m_DenseData.push_back(std::move(data));
				m_Sparse[key] = m_DenseKey.size() - 1;
			}
		}

		bool Remove(uint32_t key) {
			if (!Has(key)) {
				return false;
			}

			size_t denseIdxLast = m_DenseData.size() - 1;
			size_t denseIdxToRmv = m_Sparse[key];

			if (denseIdxToRmv != denseIdxLast) {
				// The sparse for the last item in dense now maps to the one to be removed/overwritten
				m_Sparse[m_DenseKey[denseIdxLast]] = denseIdxToRmv;

				m_DenseKey[denseIdxToRmv] = m_DenseKey[denseIdxLast];
				m_DenseData[denseIdxToRmv] = std::move(m_DenseData[denseIdxLast]);
			}

			m_DenseKey.pop_back();
			m_DenseData.pop_back();

			m_Sparse[key] = SIZE_MAX;

			return true;
		}

		//T& Get(uint32_t key) {

		//}

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

