#include "game.hpp"
#include <cstdlib>


int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Falling Sand Engine");
    SetTargetFPS(60);

    InitGrid();

    static Color FIRE_LUT[31]; // Look-up Table for fire
    static bool lut_initialised = false;

    if (!lut_initialised) {
        const Color FIRE_CORE  = { 255, 240, 150, 255 }; // Bright Yellow / White Core
        const Color FIRE_MID   = { 255, 165, 0, 255 }; // Deep Orange
        const Color FIRE_EMBER = { 150, 20, 0, 255 }; // Dark Red

        for (int life = 0; life <= 30; ++life) {
            float lifeRatio = life > 30 ? 1.0f : static_cast<float>(life) / 30.0f;
            if (lifeRatio > 0.5f) {
                float t = (1.0f - lifeRatio) * 2.0f;
                FIRE_LUT[life] = ColorLerp(FIRE_CORE, FIRE_MID, t);
            } else {
                float t = (0.5f - lifeRatio) * 2.0f;
                FIRE_LUT[life] = ColorLerp(FIRE_MID, FIRE_EMBER, t);
            }
        }
        lut_initialised = true;
    }

    Color* pixels = (Color*)malloc(WIDTH * HEIGHT * sizeof(Color)); // allocate a block of memory for the pixels
    Image img = GenImageColor(WIDTH, HEIGHT, BLACK); //CPU memory
    Texture2D texture = LoadTextureFromImage(img); //Loads the CPU image data into the GPU memory

    while (!WindowShouldClose()) {
        // spawn sand or water
        UpdateSimulation();
        UserInt(15);

        // For water color shift
        static float frequency = 1.0f / 300.0f;
        float water_t = (std::sin((float)currentFrame * frequency * PI)) * (std::sin((float)currentFrame * frequency * PI));

        for (int i = 0; i < WIDTH * HEIGHT; ++i) {
            Cell& cell = grid[i];
            ElementType type = cell.type;
            ElementProperties props = ELEMENT_REGISTRY[static_cast<int>(type)];

            if (type == ElementType::FIRE) {
                pixels[i] = FIRE_LUT[cell.life];
            } 
            else if (type == ElementType::WATER) {
                static Color hueShift = {0, 0, 160, 255};
                pixels[i] = ColorLerp(props.color, hueShift, water_t);
            } 
            else {
                float factor = 1.0f - ((float)cell.colorOffset / 255.0f) * 0.1f;
                Color color = props.color;
                color.r = color.r * factor;
                color.g = color.g * factor;
                color.b = color.b * factor;
                pixels[i] = color;
            }
        }
        UpdateTexture(texture, pixels); // Copies the whole batch of raw bytes from the CPU RAM intod
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