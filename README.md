# ECS - Entity Component System

<p align="center">
  <img src="https://img.shields.io/badge/C++-17-blue?style=flat-square&logo=c%2B%2B&color=00599C" alt="C++17">
  <img src="https://img.shields.io/badge/Pattern-ECS-green?style=flat-square" alt="ECS">
</p>

<p align="center">
  <strong>A Simple Entity Component System in C++</strong>
</p>

<p align="center">
  Data-oriented design for high-performance game object management.
</p>

---

## Features

- **Component Pools** — Efficient contiguous memory storage for components
- **Entity Bitmasks** — 32-bit component masks for fast entity queries
- **Fast Queries** — Iterate only entities with specific components
- **Automatic Memory Management** — Object pooling and recycling
- **Memory Layout Visualization** — Debug memory layout with hex dumps
- **Template-Based** — Type-safe component access

---

## Tech Stack

| Component | Technology |
|-----------|------------|
| **Language** | C++17 |
| **Architecture** | Data-Oriented Design (DOD) |
| **Storage** | Contiguous arrays (cache-friendly) |
| **IDs** | 16-bit entity IDs with bitmasks |
| **Macros** | `TRANSFORM`, `PHYSICS`, `HEALTH`, `SPRITE` |

---

## Quick Start

### Build

```bash
# Compile
g++ -std=c++17 -o ecs ecs.cpp

# Run
./ecs
```

### Basic Usage

```cpp
#include "ecs.h"

int main() {
    // Create an entity
    auto entity = ecs::ecs.entity().checkout();

    // Assign components
    auto health = ecs::ecs.component().assign<HEALTH>(entity);
    auto transform = ecs::ecs.component().assign<TRANSFORM>(entity);

    // Set component data
    health->health = 100;
    health->max = 100;
    
    transform->pos = Vec2f(10.0f, 20.0f);
    transform->scale = Vec2f(1.0f, 1.0f);

    // Query entities with specific components
    for (auto& e : ecs::ecs.query<HEALTH, TRANSFORM>()) {
        auto h = ecs::ecs.component().fetch<HEALTH>(e);
        auto t = ecs::ecs.component().fetch<TRANSFORM>(e);
        
        std::cout << "Entity " << e << " has " << h->health << " HP at ("
                  << t->pos.getX() << ", " << t->pos.getY() << ")\n";
    }

    // Return entity to pool (recycles ID)
    ecs::ecs.entity().ret(entity);
    
    return 0;
}
```

---

## Architecture

### Core Concepts

```
┌─────────────────────────────────────────────────────────┐
│                    ECS Architecture                       │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐  │
│  │  Entity 1   │    │  Entity 2   │    │  Entity 3   │  │
│  │  ID: 0      │    │  ID: 1      │    │  ID: 2      │  │
│  │  Mask: 0b11 │    │  Mask: 0b01 │    │  Mask: 0b10 │  │
│  │  (H+T)      │    │  (T only)   │    │  (H only)   │  │
│  └──────┬──────┘    └──────┬──────┘    └──────┬──────┘  │
│         │                  │                  │         │
│         └──────────────────┴──────────────────┘         │
│                            │                            │
│  ┌─────────────────────────┼─────────────────────────┐  │
│  │       Component Pools   │                         │  │
│  │  ┌─────────────────┐    │    ┌─────────────────┐   │  │
│  │  │ Transform Pool  │◄───┴────│   [0] [1] ...   │   │  │
│  │  │  Contiguous     │         │  pos,scale,rot  │   │  │
│  │  │  Memory         │         └─────────────────┘   │  │
│  │  └─────────────────┘                                 │  │
│  │  ┌─────────────────┐    ┌─────────────────┐         │  │
│  │  │  Health Pool    │◄───│   [0] ...       │         │  │
│  │  │  Contiguous     │    │  health,max    │         │  │
│  │  │  Memory         │    └─────────────────┘         │  │
│  │  └─────────────────┘                                 │  │
│  └────────────────────────────────────────────────────┘  │
│                                                         │
└─────────────────────────────────────────────────────────┘

H = Health Component, T = Transform Component
```

### Component Bitmasks

Each entity has a 32-bit mask representing which components it owns:

| Bit | Component | Description |
|-----|-----------|-------------|
| 0 | `TRANSFORM` | Position, scale, rotation |
| 1 | `HEALTH` | Health points, max health |
| 2 | `PHYSICS` | Velocity, acceleration, mass |
| 3 | `SPRITE` | Texture reference |
| ... | (reserved) | Up to 32 components |

---

## Built-in Components

| Component | Fields | Description |
|-----------|--------|-------------|
| `Transform` | `pos`, `scale`, `rotation` | 2D transform data |
| `Physics` | `vel`, `acc`, `mass` | Physics simulation data |
| `Health` | `health`, `max` | Hit points and maximum |
| `Sprite` | `texture` | Visual representation |

### Vec2f - 2D Vector

```cpp
Vec2f v(10.0f, 20.0f);
float x = v.getX();  // 10.0
float y = v.getY();  // 20.0

// Operators
Vec2f a = v + Vec2f(5, 5);  // (15, 25)
Vec2f b = v * 2.0f;          // (20, 40)
Vec2f c = -v;                // (-10, -20)
float d = v[0];              // 10.0 (x component)
```

---

## Query System

Query entities by their component mask:

```cpp
// Query all entities with both HEALTH and TRANSFORM
for (auto& e : ecs::ecs.query<HEALTH, TRANSFORM>()) {
    // Process entity
}

// Query entities with PHYSICS only
for (auto& e : ecs::ecs.query<PHYSICS>()) {
    // Update physics
}

// Query entities with all four components
for (auto& e : ecs::ecs.query<TRANSFORM, PHYSICS, HEALTH, SPRITE>()) {
    // Full game object processing
}
```

### Query Performance

- **O(1)** component fetch (array index)
- **O(N)** query iteration (N = entity count)
- Early exit via bitmask check (no virtual calls)

---

## Memory Layout

The ECS uses contiguous arrays for cache-friendly access:

```
Health Component Pool Memory Layout:

Address    Data
─────────────────────────────────────────
0x1000    [Entity 0] health=100, max=100
0x1008    [Entity 1] health=50, max=100
0x1010    [Entity 2] health=75, max=150
...

Cache Line (64 bytes) fits 8 Health components
→ Sequential iteration = minimal cache misses
```

---

## Project Structure

```
ecs/
├── ecs.h              # Main header with all implementations
├── ecs.cpp            # Example usage (optional)
└── README.md          # This file
```

### Configuration

Edit these macros in `ecs.h`:

```cpp
#define MAX_COMPONENTS 32    // Max component types (bitmask size)
#define MAX_ENTITIES 100     // Max concurrent entities
```

---

## Design Philosophy

This implementation follows the [Let's Build an ECS](https://devlog.hexops.com/2022/lets-build-ecs-part-1/) series:

1. **Data-Oriented** — Components stored contiguously
2. **Cache-Friendly** — Minimal pointer chasing
3. **Simple** — Single header, no external dependencies
4. **Fast** — Bitmask queries, direct array access

---

## Contributing

Contributions welcome!

1. Fork the repository
2. Create a feature branch
3. Add your improvements
4. Test with the example
5. Submit a Pull Request

---

## License

MIT License

---

<p align="center">
  Data-oriented game development
</p>
