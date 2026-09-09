#include "game.hpp"
#include <cstdlib>

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Falling Sand Engine");
    SetTargetFPS(60);

    InitGrid();

    Color* pixels = (Color*)malloc(WIDTH * HEIGHT * sizeof(Color)); // allocate a block of memory for the pixels
    Image img = GenImageColor(WIDTH, HEIGHT, BLACK); //CPU memory
    Texture2D texture = LoadTextureFromImage(img); //Loads the CPU image data into the GPU memory

    while (!WindowShouldClose()) {
        // spawn sand or water
        UpdateSimulation();
        UserInt(15);

        for (int i = 0; i < WIDTH * HEIGHT; ++i) {
            ElementType t = grid[i].type;
            pixels[i] = ELEMENT_REGISTRY[static_cast<int>(t)].color;
        }
        UpdateTexture(texture, pixels); // Copies the whole batch of raw bytes from the CPU RAM into
        // into the GPU's VRAM texture buffer

        BeginDrawing();
            ClearBackground(BLACK);
            
            // Stretch the low-res texture (each pixel is now 4 times as large)
            DrawTexturePro(
                texture,
                Rectangle{ 0, 0, (float)WIDTH, (float)HEIGHT},
                Rectangle{ 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT},
                Vector2{ 0, 0 },
                0.0f,
                WHITE
            );

            DrawFPS(10, 10); // show the FPS
        EndDrawing();
    }


    free(pixels);
    UnloadTexture(texture);
    CleanupGrid();
    CloseWindow();

    return 0;
}