#pragma once

#include "src/Container/SparseSet.h"
#include "Entity.h"

namespace NXTN {
    // Sparse set wrappers    
	class IComponentStorage {
    public:
        virtual ~IComponentStorage() {}
        virtual void Add(EntityID eid) = 0;
        virtual void Remove(EntityID eid) = 0;
	};

    template <typename T>
    class ComponentStorage final : IComponentStorage {
    public:
        virtual void Add(EntityID eid) override {
            m_Set.Add(eid)
        }
    private:
        SparseSet<T> m_Set;
    };
}