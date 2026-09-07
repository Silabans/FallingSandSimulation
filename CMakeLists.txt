cmake_minimum_required(VERSION 3.14)
project(FallingSandSimulation CXX)

set(CMAKE_CXX_STANDARD 17)

# Fetch Raylib automatically
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.0
)
FetchContent_MakeAvailable(raylib)

# Define your executable and its source files
add_executable(FallingSandSimulation main.cpp)

# Link Raylib to your executable target
target_link_libraries(FallingSandSimulation PRIVATE raylib)