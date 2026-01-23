#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>

namespace NXTN {
	template<typename T>
	class SparseSet
	{
	public:
		SparseSet() {}
		virtual ~SparseSet() {}

		constexpr size_t Size() const {
			return m_DenseKey.size();
		}

		bool Has(uint32_t key) const {
			if (key < m_Sparse.size()) {
				return m_Sparse[key] < SIZE_MAX;
			}
			return false;
		}

		bool Add(uint32_t key) {
			if (Has(key)) {
				return false;
			}
			if (key < m_Sparse.size()) {
				m_DenseKey.push_back(key);
				m_DenseData.emplace_back();
				m_Sparse[key] = m_DenseKey.size() - 1;
			}
			else {
				m_Sparse.resize(static_cast<size_t>(key) + 1, SIZE_MAX);

				m_DenseKey.push_back(key);
				m_DenseData.emplace_back();
				m_Sparse[key] = m_DenseKey.size() - 1;
			}
		}

		void Update(uint32_t key, T& data) {
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
			if (Has(key)) {
				size_t last = m_DenseData.size() - 1;

				m_Sparse[m_DenseKey[last]] = m_Sparse[key];

				m_DenseKey[m_Sparse[key]] = m_DenseKey[last];
				m_DenseKey.pop_back();

				m_DenseData[m_Sparse[key]] = std::move(m_DenseData[last]);
				m_DenseData.pop_back();

				m_Sparse[key] = SIZE_MAX;

				return true;
			}
			return false;
		}

	private:
		// The key type is consistent with EntityID
		std::vector<uint32_t> m_DenseKey;
		std::vector<T> m_DenseData;
		std::vector<size_t> m_Sparse;
	};
}

