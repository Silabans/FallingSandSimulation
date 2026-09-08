#pragma once
#include <optional>
#include <cmath>
#include "grid.hpp"


inline float gravity = 1.0f;

// Forward declarations
void UpdateCell(int x, int y);
void UpdateSandPhysics(int x, int y, const ElementProperties& props, Cell& cell);
void UpdateLiquidPhysics(int x, int y, const ElementProperties& props, Cell& cell);
void UpdateGasPhysics(int x, int y, const ElementProperties& props, Cell& cell);
void ReactAcid(int x, int y, int targetX, int targetY, const ElementProperties& acidProps, const ElementProperties& targetProps);
void ReactFire(int x, int y, int targetX, int targetY, const ElementProperties& acidProps, const ElementProperties& targetProps);


void UpdateSimulation() {
    for (int y = HEIGHT - 1; y >= 0; --y) {
        // alternate the sweep order (left-to-right, then right-to-left)
        if (currentFrame % 2 == 0) { 
            for (int x = 0; x < WIDTH; ++x) {
                UpdateCell(x, y);
            }
        } else {
            for (int x = WIDTH - 1; x >= 0; --x) {
                UpdateCell(x, y);
            }
        }
    }
    currentFrame++;
}

void UpdateCell(int x, int y) {
    Cell& cell = grid[y * WIDTH + x];
    if (cell.type == ElementType::EMPTY || cell.lastFrame == currentFrame) return;

    // casts ElementType into its enum value (eg: EMPTY is the first type in the struct -> enum value is 0)
    const ElementProperties& props = ELEMENT_REGISTRY[static_cast<size_t>(cell.type)];

    static const int dx[] = {0, 0, -1, 1};
    static const int dy[] = {-1, 1, 0, 0};

    // scan neighbours to check for reactions
    for (int i = 0; i < 4; ++i) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (!inBounds(nx, ny)) continue;
        const int ncoor = coor(nx, ny);

        ElementType targetType = grid[ncoor].type;
        const ElementProperties& targetProps = ELEMENT_REGISTRY[static_cast<int>(targetType)];

        if (targetType == ElementType::ACID) {
            ReactAcid(nx, ny, x, y, targetProps, props);
        }

        if (cell.type == ElementType::FIRE) {
            ReactFire(x, y, nx, ny, props, targetProps);
        }
    }

    if (cell.type == ElementType::EMPTY) return;

    switch (props.movement) {
        case MovementType::IMMOVABLE:
            break;
        case MovementType::SAND_LIKE:
            UpdateSandPhysics(x, y, props, cell);
            break;
        case MovementType::LIQUID:
            UpdateLiquidPhysics(x, y, props, cell);
            break;
        case MovementType::GAS:
            UpdateGasPhysics(x, y, props, cell);
            break;
    }
}


void UpdateSandPhysics(int x, int y, const ElementProperties& props, Cell& cell) {
    int ny = y;
    for (int i = cell.speed; i >= 1; --i) {
        if (isEmpty(x, y + i)) {
            ny = y + i;
            break;
        }
    }
    if (ny != y) {
        moveCell(x, y, x, ny);
        cell.speed += gravity;
    }
    
    bool goLeft = GetRandomValue(0, 1) == 0;
    int defaultDir = goLeft ? -1 : 1;
    int secondaryDir = goLeft ? 1 : -1;

    if (inBounds(x, y + 1)) {
        int end = 15;
        int shift = GetRandomValue(-1, end);
        if (shift == end) shift = 1;
        else if (shift > -1 && shift < end) shift = 0;

        if (inBounds(x + shift, y + 1)) {
            ElementProperties below_props = ELEMENT_REGISTRY[static_cast<int>(grid[coor(x+shift, y+1)].type)];
            if (below_props.density < props.density) {
                swapCell(x, y, x+shift, y + 1);
                return;
            }
        }
    }

    if (isEmpty(x+defaultDir, y + 1)) {
        moveCell(x, y, x + defaultDir, y + 1);
    } else if (isEmpty(x+secondaryDir, y + 1)) {
        moveCell(x, y, x + secondaryDir, y + 1);
    } else cell.speed = 0;
}


void UpdateLiquidPhysics(int x, int y, const ElementProperties& props, Cell& cell) {
    if (isEmpty(x, y + 1) && cell.speed >= 4) {
        moveCell(x, y, x, y + cell.speed / 4);
        cell.speed += 1;
    }
    
    if (inBounds(x, y + 1)) {
        int end = 9;
        int shift = GetRandomValue(-1, end);
        if (shift == end) shift = 1;
        else if (shift > -1 && shift < end) shift = 0;

        if (inBounds(x + shift, y + 1)) {
            ElementProperties below_props = ELEMENT_REGISTRY[static_cast<int>(grid[coor(x+shift, y+1)].type)];
            if (below_props.density < props.density) {
                swapCell(x, y, x+shift, y + 1);
                return;
        }
        }
    }

    bool goLeft = GetRandomValue(1, 2) == 1;
    int defaultDir = goLeft ? -1 : 1;
    int secondaryDir = goLeft ? 1 : -1;

    if (isEmpty(x + defaultDir, y + 1)) {
        moveCell(x, y, x + defaultDir, y + 1);
        return;
    } else if (isEmpty(x + secondaryDir, y + 1)) {
        moveCell(x, y, x + secondaryDir, y + 1);
        return;
    }

    cell.speed = 0;
    int nx = x;
    int steps = props.dispersionRate;

    for (int i = 1; i <= steps; ++i) {
        int checkX = x + (i * defaultDir);
        if (isEmpty(checkX, y) && !isEmpty(checkX, y + 1)) {
            nx = checkX;
        } else break;
    }

    if (nx != x) {
        moveCell(x, y, nx, y);
        return;
    }

    for (int i = 1; i <= steps; ++i) {
        int checkX = x + (i * secondaryDir);
        if (isEmpty(checkX, y) && !isEmpty(checkX, y + 1)) {
            nx = checkX;
        } else break;
    }

    if (nx != x) {
        moveCell(x, y, nx, y);
        return;
    }
}


void UpdateGasPhysics(int x, int y, const ElementProperties& props, Cell& cell) {
    // Fire lifetime decay logic
    if (cell.type == ElementType::FIRE) {
        if (cell.life > 0) {
            if (GetRandomValue(1, 100) > 15) cell.life--;
        } else {
            // Turn dead fire into smoke with a high probability
            if (GetRandomValue(1, 100) <= 70) {
                grid[coor(x, y)] = Cell{ ElementType::SMOKE, 15, 15, (uint8_t)currentFrame, 1 };
            } else {
                grid[coor(x, y)] = Cell{ ElementType::EMPTY, 0, 0, 0, 0 };
            }
            return;
        }

        // Throttle fire movement rate
        if (currentFrame % 3 != 0) return;
    }

    // Pick a single random horizontal direction for this frame pass
    int spread = GetRandomValue(-props.dispersionRate, props.dispersionRate);
    int xspread = (spread == 0.0f) ? 0.0f : spread / std::abs(spread);
    
    int yspread = (std::abs(spread) - 10 <= 0) ? -1 : 0;

    // Upward Movement (Directly or Diagonally)
    if (inBounds(x + xspread, y + yspread) && isEmpty(x + xspread, y + yspread)) {
        moveCell(x, y, x + xspread, y + yspread);
        return;
    }

    int dir = (GetRandomValue(0, 1) == 0) ? -1 : 1;
    if (inBounds(x + dir, y - 1) && isEmpty(x + dir, y - 1)) {
        moveCell(x, y, x + dir, y - 1);
        return;
    }
    if (inBounds(x - dir, y - 1) && isEmpty(x - dir, y - 1)) {
        moveCell(x, y, x - dir, y - 1);
        return;
    }


    // Local Diffusion (Single-step Brownian Motion)
    if (inBounds(x + dir, y) && isEmpty(x + dir, y)) {
        moveCell(x, y, x + dir, y);
        return;
    }
    if (inBounds(x - dir, y) && isEmpty(x - dir, y)) {
        moveCell(x, y, x - dir, y);
        return;
    }
}



// Reactions
void ReactAcid(int x, int y, int targetX, int targetY, const ElementProperties& acidProps, const ElementProperties& targetProps) {
    Cell& acid = grid[coor(x, y)];
    Cell& target = grid[coor(targetX, targetY)];

    if (target.type == ElementType::ACID || target.type == ElementType::EMPTY) return;
    if (targetProps.acidResistance >= 1.0f) return; // for materials that are immune to acid

    if (GetRandomValue(1, 100) > 30) return; // 30 percent chance only for acid to react

    float damage = 5.0f * (1 - targetProps.acidResistance);
    target.health -= damage;
    acid.life -= 1; // Acid is consumed overtime as it burns the target

    if (target.health <= 0.0f) {
        grid[coor(targetX, targetY)]= Cell{ ElementType::EMPTY, 0, 0, (uint8_t)currentFrame, 0 }; // reset the cell
    }
    if (acid.life <= 0) {
        grid[coor(x, y)] = Cell{ ElementType::EMPTY, 0, 0, (uint8_t)currentFrame };
    }
}


void ReactFire(int x, int y, int targetX, int targetY, const ElementProperties& fireProps, const ElementProperties& targetProps) {
    Cell& fire = grid[coor(x, y)];
    Cell& target = grid[coor(targetX, targetY)];

    if (target.type == ElementType::FIRE) return;
    if (!targetProps.isFlammable) return;

    if (GetRandomValue(1, 100) > 90) return;

    fire.life -= 10.0f;
    target.health -= 5.0f;

    if (target.health <= 0.0f) {
        grid[coor(targetX, targetY)] = Cell{ ElementType::FIRE, 15, 0, (uint8_t)currentFrame, 1 };
    }

    if (fire.life <= 0.0f) {
        grid[coor(x, y)] = Cell{ ElementType::SMOKE, 15, 0, (uint8_t)(currentFrame - 1), 1 };
    }

}


void UserInt(int radius) {
    Vector2 m = GetMousePosition();
    int mx = static_cast<int>(m.x) / SCALE;
    int my = static_cast<int>(m.y) / SCALE;

    std::optional<ElementType> type;
    if (IsKeyDown(KEY_S)) type = ElementType::SAND;
    else if (IsKeyDown(KEY_W)) type = ElementType::WATER;
    else if (IsKeyDown(KEY_A)) type = ElementType::ACID;
    else if (IsKeyDown(KEY_E)) type = ElementType::EMPTY;
    else if (IsKeyDown(KEY_D)) type = ElementType::WOOD;
    else if (IsKeyDown(KEY_G)) type = ElementType::SMOKE;
    else if (IsKeyDown(KEY_F)) type = ElementType::FIRE;

    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            if ((std::abs(dy) + std::abs(dx) - (radius / 2)) > radius) continue;
            if (inBounds(mx + dx, my + dy) and type.has_value()) {
                const ElementProperties& props = ELEMENT_REGISTRY[static_cast<int>(type.value())];
                Cell& c = grid[coor(mx + dx, my + dy)];
                c.type = type.value();
                c.health = props.maxHealth;
                c.life = GetRandomValue(15, 50);
                c.lastFrame = currentFrame > 0 ? currentFrame - 1 : 0; // set to 1 frame before -> so that it gets processed immediately
                c.speed = 1;
            }
        }
    }
}