#include "ecs.h"
#include <ctime>
#include <random>
#include <cstdint>
#include <bitset>
#include <memory>
#include <vector>
#include <array>
#include <deque>
#include <iostream>
#include <set>
#include <iomanip>

Vec2f::Vec2f() : v{0, 0} {}

Vec2f::Vec2f(float x, float y) : v{x, y} {}

float Vec2f::getX() const
{
    return v[0];
}

float Vec2f::getY() const
{
    return v[1];
}

void Vec2f::setX(float x)
{
    v[0] = x;
}

void Vec2f::setY(float y)
{
    v[1] = y;
}

Vec2f Vec2f::operator-() const
{
    return Vec2f(-v[0], -v[1]);
}

Vec2f Vec2f::operator+(Vec2f vec) const
{
    return Vec2f(v[0] + vec.v[0], v[1] + vec.v[1]);
}

Vec2f Vec2f::operator*(float scale) const
{
    return Vec2f(getX() * scale, getY() * scale);
}

float Vec2f::operator[](int idx) const
{
    return v[idx];
}

float &Vec2f::operator[](int idx)
{
    return v[idx];
}

Vec2f &Vec2f::operator+=(const Vec2f &vec)
{
    v[0] += vec.v[0];
    v[1] += vec.v[1];
    return *this;
}

Vec2i::Vec2i() : v{0, 0} {}

Vec2i::Vec2i(int x, int y) : v{x, y} {}

int Vec2i::getX() const
{
    return v[0];
}

int Vec2i::getY() const
{
    return v[1];
}

void Vec2i::setX(int x)
{
    v[0] = x;
}

void Vec2i::setY(int y)
{
    v[1] = y;
}

Vec2i Vec2i::operator-() const
{
    return Vec2i(-v[0], -v[1]);
}

Vec2i Vec2i::operator+(Vec2i vec) const
{
    return Vec2i(v[0] + vec.v[0], v[1] + vec.v[1]);
}

Vec2f Vec2i::operator*(float scale) const
{
    return Vec2f(getX() * scale, getY() * scale);
}

int Vec2i::operator[](int idx) const
{
    return v[idx];
}

int &Vec2i::operator[](int idx)
{
    return v[idx];
}

Vec2i &Vec2i::operator+=(const Vec2i &vec)
{
    v[0] += vec.v[0];
    v[1] += vec.v[1];
    return *this;
}

namespace ecs
{
    ECS ecs;

    Entity::Entity(eid_t i, cmask_t m) : id(i), mask(m) 
    {

    }

    ComponentPool::ComponentPool(uint16_t size) : _size {size}
    {
        _ptr_data = std::make_unique<char[]>(_size * MAX_ENTITIES);
    }

    uint16_t ComponentPool::size() const
    {
        return _size;
    }

    void *ComponentPool::get(uint16_t idx)
    {
        return _ptr_data.get() + idx * _size;
    }

    EntityManager::EntityManager(uint16_t max_entities) : _max_entities{max_entities}
    {
        for (uint16_t i{0}; i < _max_entities; i++)
        {
            _entities.push_back({i, cmask_t()});
            _free.push_back(i);
        }
    }

    Entity *EntityManager::get(eid_t idx)
    {
        return &_entities[idx];
    }

    Entity *EntityManager::checkout()
    {
        if (_free.empty())
            return nullptr;
        eid_t idx{_free.front()};
        _free.pop_front();
        return get(idx);
    }

    void EntityManager::ret(Entity *&e_ptr)
    {
        e_ptr->mask.reset();
        _free.push_back(e_ptr->id);
        e_ptr = nullptr;
    }

    uint16_t EntityManager::maxEntities() const
    {
        return _max_entities;
    }

    std::vector<Entity>& EntityManager::getEntities()
    {
        return _entities;
    }

    ECS::ECS() : _entity_manager{MAX_ENTITIES} {}

    EntityManager &ECS::entity()
    {
        return _entity_manager;
    }

    ComponentManager &ECS::component()
    {
        return _component_manager;
    }

    void ECS::debug()
    {
        // ANSI color codes
        const char* RESET  = "\033[0m";
        const char* RED    = "\033[31m";
        const char* GREEN  = "\033[32m";
        const char* YELLOW = "\033[33m";
        const char* BLUE   = "\033[34m";
        const char* MAGENTA= "\033[35m";
        const char* CYAN   = "\033[36m";
        const char* WHITE  = "\033[37m";
        const char* BOLD   = "\033[1m";
        const char* DIM    = "\033[2m";

        std::cout << "\n";
        std::cout << CYAN << "+==============================================================+" << RESET << "\n";
        std::cout << CYAN << "|" << RESET << BOLD << "                    ECS MEMORY VISUALIZATION                  " << RESET << CYAN << "|" << RESET << "\n";
        std::cout << CYAN << "+==============================================================+" << RESET << "\n";

        // Build free set for O(1) lookup
        std::set<eid_t> free_set(_entity_manager._free.begin(), _entity_manager._free.end());

        // Entity array visualization
        std::cout << CYAN << "|" << RESET << " " << BOLD << "ENTITIES" << RESET << " (" << GREEN << (MAX_ENTITIES - _entity_manager._free.size()) << RESET << "/" << MAX_ENTITIES << " in use):\n";
        std::cout << CYAN << "|" << RESET << " +";
        int cols = 20;
        for (int i = 0; i < cols; i++) std::cout << "--";
        std::cout << "+\n";

        for (int row = 0; row < MAX_ENTITIES; row += cols) {
            std::cout << CYAN << "|" << RESET << " |";
            for (int col = 0; col < cols && (row + col) < MAX_ENTITIES; col++) {
                eid_t id = row + col;
                if (free_set.count(id)) {
                    std::cout << DIM << ". " << RESET;
                } else {
                    std::cout << GREEN << "# " << RESET;
                }
            }
            std::cout << "|" << DIM << " e" << row << "-" << std::min(row + cols - 1, (int)MAX_ENTITIES - 1) << RESET << "\n";
        }
        std::cout << CYAN << "|" << RESET << " +";
        for (int i = 0; i < cols; i++) std::cout << "--";
        std::cout << "+\n";
        std::cout << CYAN << "|" << RESET << "   " << GREEN << "#" << RESET << " = in use    " << DIM << "." << RESET << " = free\n";

        // Component masks for active entities
        std::cout << CYAN << "+--------------------------------------------------------------+" << RESET << "\n";
        std::cout << CYAN << "|" << RESET << " " << BOLD << "ACTIVE ENTITY MASKS:" << RESET << "\n";
        for (auto& e : _entity_manager.getEntities()) {
            if (!free_set.count(e.id)) {
                std::cout << CYAN << "|" << RESET << "   " << YELLOW << "e" << std::setw(3) << e.id << RESET << " [";
                for (int i = MAX_COMPONENTS - 1; i >= 0; i--) {
                    if (e.mask.test(i)) {
                        std::cout << GREEN << '1' << RESET;
                    } else {
                        std::cout << DIM << '0' << RESET;
                    }
                    if (i % 8 == 0 && i > 0) std::cout << ' ';
                }
                std::cout << "]\n";
            }
        }

        // Component pools visualization with actual data
        std::cout << CYAN << "+--------------------------------------------------------------+" << RESET << "\n";
        std::cout << CYAN << "|" << RESET << " " << BOLD << "COMPONENT POOLS:" << RESET << "\n";

        // Field colors for different component types
        // Pool 0 (Health): field 0 = health (green), field 1 = max (cyan)
        // Pool 1 (Transform): fields colored sequentially
        const char* field_colors[][8] = {
            {GREEN, CYAN},                              // Pool 0: Health (health, max)
            {YELLOW, MAGENTA, BLUE},                    // Pool 1: Transform (pos.x, pos.y, scale.x, scale.y, rotation)
        };

        const char* pool_colors[] = {GREEN, YELLOW, BLUE, MAGENTA, CYAN, RED};

        for (size_t cid = 0; cid < _component_manager._pools.size(); cid++) {
            auto& pool = _component_manager._pools[cid];
            if (pool == nullptr) continue;

            const char* pool_color = pool_colors[cid % 6];

            // Get component name based on pool id
            const char* comp_name = "Unknown";
            if (cid == 0) comp_name = "Health";
            else if (cid == 1) comp_name = "Transform";
            else if (cid == 2) comp_name = "Physics";
            else if (cid == 3) comp_name = "Sprite";

            std::cout << CYAN << "|" << RESET << "   " << pool_color << BOLD << "Pool[" << cid << "] " << comp_name << RESET << " (size=" << pool->size() << " bytes)\n";
            std::cout << CYAN << "|" << RESET << "   +";
            for (int i = 0; i < cols; i++) std::cout << "--";
            std::cout << "+\n";

            for (int row = 0; row < MAX_ENTITIES; row += cols) {
                std::cout << CYAN << "|" << RESET << "   |";
                for (int col = 0; col < cols && (row + col) < MAX_ENTITIES; col++) {
                    eid_t id = row + col;
                    if (!free_set.count(id)) {
                        Entity& e = _entity_manager.getEntities()[id];
                        if (e.mask.test(cid)) {
                            std::cout << pool_color << "* " << RESET;
                        } else {
                            std::cout << "  ";
                        }
                    } else {
                        std::cout << DIM << ". " << RESET;
                    }
                }
                std::cout << "|\n";
            }
            std::cout << CYAN << "|" << RESET << "   +";
            for (int i = 0; i < cols; i++) std::cout << "--";
            std::cout << "+\n";

            // Show field legend for this pool
            std::cout << CYAN << "|" << RESET << "   " << DIM << "FIELDS:" << RESET;
            if (cid == 0) {
                std::cout << " " << GREEN << "health" << RESET << " | " << CYAN << "max" << RESET;
            } else if (cid == 1) {
                std::cout << " " << YELLOW << "pos.x" << RESET << " " << MAGENTA << "pos.y" << RESET << " " << BLUE << "scale/rot" << RESET;
            }
            std::cout << "\n";

            // Show actual data for each slot (double column) with field-based coloring
            std::cout << CYAN << "|" << RESET << "   " << DIM << "DATA:" << RESET << "\n";
            int half = (MAX_ENTITIES + 1) / 2;
            for (int i = 0; i < half; i++) {
                // Left column
                std::cout << CYAN << "|" << RESET << "   ";
                if (!free_set.count(i)) {
                    Entity& e = _entity_manager.getEntities()[i];
                    if (e.mask.test(cid)) {
                        void* mem = pool->get(i);
                        unsigned char* bytes = static_cast<unsigned char*>(mem);

                        // For Health pool, check if low health for red warning
                        bool low_health = false;
                        if (cid == 0) {
                            float* health_val = reinterpret_cast<float*>(mem);
                            float* max_val = reinterpret_cast<float*>(static_cast<char*>(mem) + 4);
                            if (*max_val > 0 && (*health_val / *max_val) < 0.25f) {
                                low_health = true;
                            }
                        }

                        if (low_health) {
                            std::cout << RED << BOLD << "e" << std::setw(3) << i << RESET << ":";
                        } else {
                            std::cout << WHITE << "e" << std::setw(3) << i << RESET << ":";
                        }

                        // Color each byte based on which field it belongs to
                        for (size_t b = 0; b < pool->size(); b++) {
                            size_t field_idx = b / 4;  // Assume 4-byte fields (floats)
                            const char* byte_color = DIM;

                            if (cid < 2 && field_colors[cid][field_idx] != nullptr) {
                                byte_color = field_colors[cid][field_idx];
                            } else {
                                byte_color = pool_color;
                            }

                            // Override with red for low health
                            if (low_health && cid == 0 && field_idx == 0) {
                                byte_color = RED;
                            }

                            if (bytes[b] == 0) {
                                std::cout << DIM << " " << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)bytes[b] << RESET;
                            } else {
                                std::cout << byte_color << " " << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)bytes[b] << RESET;
                            }
                        }
                        std::cout << " ";
                    } else {
                        std::cout << DIM << "e" << std::setw(3) << i << ": (no data)      " << RESET;
                    }
                } else {
                    std::cout << DIM << "e" << std::setw(3) << i << ": (free)         " << RESET;
                }

                // Right column
                int j = i + half;
                if (j < MAX_ENTITIES) {
                    if (!free_set.count(j)) {
                        Entity& e = _entity_manager.getEntities()[j];
                        if (e.mask.test(cid)) {
                            void* mem = pool->get(j);
                            unsigned char* bytes = static_cast<unsigned char*>(mem);

                            bool low_health = false;
                            if (cid == 0) {
                                float* health_val = reinterpret_cast<float*>(mem);
                                float* max_val = reinterpret_cast<float*>(static_cast<char*>(mem) + 4);
                                if (*max_val > 0 && (*health_val / *max_val) < 0.25f) {
                                    low_health = true;
                                }
                            }

                            if (low_health) {
                                std::cout << " " << RED << BOLD << "e" << std::setw(3) << j << RESET << ":";
                            } else {
                                std::cout << " " << WHITE << "e" << std::setw(3) << j << RESET << ":";
                            }

                            for (size_t b = 0; b < pool->size(); b++) {
                                size_t field_idx = b / 4;
                                const char* byte_color = DIM;

                                if (cid < 2 && field_colors[cid][field_idx] != nullptr) {
                                    byte_color = field_colors[cid][field_idx];
                                } else {
                                    byte_color = pool_color;
                                }

                                if (low_health && cid == 0 && field_idx == 0) {
                                    byte_color = RED;
                                }

                                if (bytes[b] == 0) {
                                    std::cout << DIM << " " << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)bytes[b] << RESET;
                                } else {
                                    std::cout << byte_color << " " << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)bytes[b] << RESET;
                                }
                            }
                        } else {
                            std::cout << "  " << DIM << "e" << std::setw(3) << j << ": (no data)      " << RESET;
                        }
                    } else {
                        std::cout << "  " << DIM << "e" << std::setw(3) << j << ": (free)         " << RESET;
                    }
                }
                std::cout << std::dec << "\n";
            }
        }

        std::cout << CYAN << "+==============================================================+" << RESET << "\n";
        std::cout << "\n";
    }
}
uint16_t counter = 0;
int main() {
    std::srand(std::time(nullptr));

    auto a = ecs::ecs.entity().checkout();
    while (a != nullptr) {
        auto health = ecs::ecs.component().assign<HEALTH>(a);
        auto transform = ecs::ecs.component().assign<TRANSFORM>(a);
        health->health = static_cast<float>(std::rand() % 1000);
        health->max = static_cast<float>(1000 + (std::rand() % 500));
        a = ecs::ecs.entity().checkout();
    }

    ecs::ecs.debug();

    // Print entities with low health
    std::cout << "\n--- LOW Health Entities (< 25% of max) ---\n";
    for (auto& ptr : ecs::ecs.query<HEALTH,TRANSFORM>()) {
        auto health = ecs::ecs.component().fetch<HEALTH>(ptr);
        if (health->health < health->max * 0.25f) {
            std::cout << "[LOW!] e" << ptr->id << " health=" << health->health << "/" << health->max << "\n";
        }
    }
}
