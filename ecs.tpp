#include "ecs.h"

namespace ecs
{
    template <typename T>
    T* ComponentManager::assign(Entity *e_ptr)
    {
        if (e_ptr == nullptr)
            return nullptr;
        uint16_t cid = getId<T>();
        // DEBUG_LOG("component (%d) -> entity (address|ID): %p | %d\n",cid,e_ptr,e_ptr->id);
        if (e_ptr->mask.test(cid))
        {
            //  DEBUG_LOG("reassignment for component (%d) to entity ID: %d, returning null\n",cid, e_ptr->id);
            return nullptr;
        }
        if (_pools.size() <= MAX_COMPONENTS)
        {
            uint16_t n = _pools.size() + 1;
            // DEBUG_LOG("component pool (%d) to: %d -> %d\n", cid, _pools.size(), n);
            _pools.resize(n);
        }
        if (_pools[cid] == nullptr)
            _pools[cid] = std::make_unique<ComponentPool>(sizeof(T));
        void *mem = _pools[cid]->get(e_ptr->id);
        T *ptr_component = new (mem) T();
        e_ptr->mask.set(cid);
        return ptr_component;
    }

    template <typename T>
    T* ComponentManager::fetch(Entity *e_ptr)
    {
        if (e_ptr == nullptr)
            return nullptr;
        uint16_t cid = getId<T>();
        if (!ComponentManager::has<T>(e_ptr))
            return nullptr;
        void *mem = _pools[cid]->get(e_ptr->id);
        return reinterpret_cast<T *>(mem);
    }

    template <typename T>
    bool has(Entity *e_ptr, T)
    {
        if (e_ptr == nullptr)
            return false;
        uint16_t cid = getId<T>();
        return e_ptr->mask.test(cid);
    }
    template <typename T, typename... Ts>
    bool has(Entity *e_ptr, T, Ts... ts)
    {
        return has(e_ptr, T()) && has(e_ptr, ts...);
    }

    template <typename... T>
    bool ComponentManager::has(Entity *e_ptr)
    {
        return has(e_ptr, T()...);
    }
}
