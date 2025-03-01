#include <bitset>
#include <cstdint>
#include <memory>
#include <vector>
#define MAX_ENTITIES 1000
#define MAX_COMPONENTS 32
typedef uint16_t EntityID;
namespace ecs {
    struct Sprite {
        char* texture {nullptr};
    };
    class ComponentPool {
        private:
            std::unique_ptr<char[]> ptr_data;
            uint16_t _size;
        public:
            ComponentPool(uint16_t s) : _size {s} {
                ptr_data = std::make_unique<char[]>(_size * MAX_ENTITIES);
            }

            uint16_t size() const {
                return _size;
            }

            void* get(uint16_t idx) {
                return ptr_data.get() + idx * _size;
            }
    };
    int counter = 0;
    template <class T>
    size_t getId() {
        static int cid = counter++;
        return cid;
    }
    class Scene 
    {
        private:
            std::vector<std::unique_ptr<ComponentPool>> pools;
        public:
            template <class T>
            T* assign(EntityID id) {
                size_t cid = getId<T>();
                if (pools.size() <= MAX_COMPONENTS) {
                    pools.resize(pools.size() + 1);
                }
                if (pools[cid] == nullptr) {
                    pools[cid] = std::make_unique<ComponentPool>(sizeof(T));
                }
                void* mem = pools[cid]->get(id);
                T* ptr_component = new (mem) T();
                return ptr_component;
            }
    };
}

int main() {
    ecs::Scene s;
    s.assign<ecs::Sprite>(0);
    s.assign<ecs::Sprite>(1);
}