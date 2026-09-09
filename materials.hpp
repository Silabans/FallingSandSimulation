#pragma once
#include <cstdint>
#include <raylib.h>

// for type checks
enum class ElementType : uint8_t { // each type takes up 1 byte instead the default 4 bytes
    EMPTY = 0,
    SAND,
    WATER,
    ACID,
    WOOD,
    SMOKE,
    FIRE,
    STEAM
};

enum class MovementType : uint8_t {
    IMMOVABLE,
    SAND_LIKE, // slides diagonally down
    LIQUID, // slides horizontally
    GAS // rises upward and diffuses (brownian motion)
};


// Data container
struct ElementProperties {
    const char* name;
    MovementType movement;
    Color color;
    float density;
    bool isFlammable;
    float acidResistance;
    float maxHealth; // base durability
    int dispersionRate; // dictates how fast a liquid flows / gas diffuses
};


inline const ElementProperties ELEMENT_REGISTRY[] {
    {"Empty", MovementType::IMMOVABLE, {0, 0, 0, 0}, 0.0f, false, 0.1f, 0.0f, 0},
    {"Sand", MovementType::SAND_LIKE, {230, 190, 100, 255}, 0.7f, false, 0.5f, 20.0f, 0},
    {"Water", MovementType::LIQUID, {50, 100, 230, 240}, 0.4f, false, 0.2f, 10.0f, 10},
    {"Acid", MovementType::LIQUID, {100, 230, 50, 240}, 0.6f, false, 1.0f, 10.0f, 4},
    {"Wood", MovementType::IMMOVABLE, {101, 67, 33, 200}, 0.9f, true, 0.3f, 20.0f, 0},
    {"Smoke", MovementType::GAS, {80, 80, 80, 255}, 0.05f, false, 0.5f, 5.0f, 2},
    {"Fire", MovementType::GAS, {255, 100, 0, 255}, 0.2f, false, 1.0f, 5.0f, 3},
    {"Steam", MovementType::GAS, {255, 255, 255, 220}, 0.1f, false, 0.5f, 5.0f, 4}
};
