#pragma once
#include "tools.hpp"
#include "materials.hpp"
#include <utility> // for std::swap


struct Cell {
    ElementType type = ElementType::EMPTY;
    int health;
    uint8_t life = 15; // fire lifetime, acid strength, etc
    uint8_t lastFrame = 0; // prevents double updating cell in one iteration
    int speed; // for an accelerating fall
};


// using smart pointers to prevent stack overflow
inline Cell* grid = nullptr;

inline void InitGrid() { // initialise the grid in a heap instead of a stack (prevents stackoverflow)
    grid = new Cell[WIDTH * HEIGHT]; // array of such parameters
}

inline void CleanupGrid() {
    delete[] grid;
}


void moveCell(int x, int y, int nx, int ny) {
    grid[coor(nx, ny)] = grid[coor(x, y)];
    grid[coor(x, y)] = Cell{ ElementType::EMPTY, 0, 0, (uint8_t)currentFrame, 0};

    grid[coor(nx, ny)].lastFrame = currentFrame;
}

void swapCell(int x1, int y1, int x2, int y2) {
    std::swap(grid[coor(x1, y1)], grid[coor(x2, y2)]);

    grid[coor(x1, y1)].lastFrame = currentFrame;
    grid[coor(x2, y2)].lastFrame = currentFrame;
}


bool inBounds(int x, int y) {
    return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}


bool isEmpty(int x, int y) {
    if (!inBounds(x, y)) return false; // the boundaries are static walls (not empty)
    return grid[y * WIDTH + x].type == ElementType::EMPTY;
}


void ResolveFire(int x, int y, Cell& cell) {
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
}


void ResolveTemporaryGas(int x, int y, Cell& cell) {
    if (cell.type == ElementType::STEAM) {
        if (cell.life > 0) {
            if (GetRandomValue(1, 100) > 50) --cell.life;
        } else {
            if (GetRandomValue(1, 100) > 60) grid[coor(x, y)] = Cell{ ElementType::EMPTY, 0, 0, 0, 0 };
            else grid[coor(x, y)] = Cell{ ElementType::WATER, 15, 15, (uint8_t)(currentFrame - 1), 1 };
        }
    }
    if (cell.type == ElementType::SMOKE) {
        if (cell.life > 0) {
            if (GetRandomValue(1, 100) > 97) --cell.life;
        } else {
            grid[coor(x, y)] = Cell{ ElementType::EMPTY, 0, 0, 0, 0 };
        }
    }
}