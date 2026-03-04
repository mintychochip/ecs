# ECS

A basic Entity Component System in C++.

## Usage

```cpp
#include "ecs.h"

int main() {
    // Create an entity
    auto entity = ecs::ecs.entity().checkout();

    // Assign components
    auto health = ecs::ecs.component().assign<HEALTH>(entity);
    auto transform = ecs::ecs.component().assign<TRANSFORM>(entity);

    health->health = 100;

    // Query entities
    for (auto& e : ecs::ecs.query<HEALTH, TRANSFORM>()) {
        auto h = ecs::ecs.component().fetch<HEALTH>(e);
    }

    // Return entity
    ecs::ecs.entity().ret(entity);
}
```

## Components

- `Transform` - pos, scale, rotation
- `Physics` - vel, acc, mass
- `Health` - health, max
- `Sprite` - texture

## Build

```bash
g++ -o ecs ecs.cpp
```
