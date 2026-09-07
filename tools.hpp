#pragma once
#include <random>


inline int currentFrame = 0;
const inline int SCREEN_WIDTH = 1920;
const inline int SCREEN_HEIGHT = 1080;

const inline int SCALE = 4;
const inline int WIDTH = SCREEN_WIDTH / SCALE;
const inline int HEIGHT = SCREEN_HEIGHT / SCALE;

// gives the actual index of a cell in the 1d grid by taking in their x-y coordinates
inline int coor(int x, int y) {
    return y * WIDTH + x;
}