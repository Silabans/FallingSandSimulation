#pragma once
#include <random>


inline uint8_t currentFrame = 0;
const inline int SCREEN_WIDTH = 1760;
const inline int SCREEN_HEIGHT = 990;

const inline int SCALE = 3;
const inline int WIDTH = SCREEN_WIDTH / SCALE;
const inline int HEIGHT = SCREEN_HEIGHT / SCALE;

// gives the actual index of a cell in the 1d grid by taking in their x-y coordinates
inline int coor(int x, int y) {
    return y * WIDTH + x;
}