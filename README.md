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
## Methodology

Entity-Component-System's (ECS) generally follow the same format, have some sort of entity ID represent an entity and have arrays with objects represent the data entities own.
This repository is a simple implementation of an ECS and loosely models this article: https://devlog.hexops.com/2022/lets-build-ecs-part-1/

## Results

Entity Bitmasks, we had two components in the code, Health (1 bit) and Transform (0 bit). All entities had both components.
<img width="222" height="724" alt="Screenshot 2026-03-04 011945" src="https://github.com/user-attachments/assets/d13fd38a-2392-4ae7-bdd4-6359a011c9e3" />

Sample Hexdump of the health component pool, all entities had randomized health values.
<img width="329" height="554" alt="Screenshot 2026-03-04 011938" src="https://github.com/user-attachments/assets/bfdb5bf3-d036-44c1-bc38-32a4b88f25fc" />
