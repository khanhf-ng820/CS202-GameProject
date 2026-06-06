cmake_minimum_required(VERSION 3.20)
project(PvZGame LANGUAGES CXX)

# Force CMake 4.0+ to treat legacy third-party configurations as CMake 3.5+
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# Force the modern C++20 Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include CMake's FetchContent module to automate dependency retrieval
include(FetchContent)

# ---------------------------------------------------------
# Dependency: raylib
# ---------------------------------------------------------
# Turn off building examples and games bundled with the raylib source code
set(BUILD_EXAMPLES OFF CACHE BOOL "Disable raylib examples" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "Disable raylib games" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.0 # Compiles via the stable v5.0 milestone
	GIT_SHALLOW    TRUE # Only fetch the latest commit for the specified tag to speed up the process
)

# ---------------------------------------------------------
# Dependency: raygui (Immediate Mode GUI Companion Tool)
# ---------------------------------------------------------
FetchContent_Declare(
    raygui
    GIT_REPOSITORY https://github.com/raysan5/raygui.git
    GIT_TAG        4.0 # Pin to stable v4.0 to match raylib 5.0 seamlessly
	GIT_SHALLOW    TRUE # Only fetch the latest commit for the specified tag to speed up the process
)

# Download and load both libraries into the build environment
FetchContent_MakeAvailable(raylib raygui)

# ---------------------------------------------------------
# Target Configuration
# ---------------------------------------------------------
# Locate all source files inside the src directory
file(GLOB_RECURSE SRC_FILES src/*.cpp)

# Define the primary game executable target
add_executable(${PROJECT_NAME} ${SRC_FILES})

# raygui is a header-only library; wrap its include directory as an interface target
add_library(raygui_interface INTERFACE)
target_include_directories(raygui_interface INTERFACE ${raygui_SOURCE_DIR}/src)

# Link dependencies to your game target
# Note: raylib handles system graphics layers (OpenGL, GLFW) transparently under the hood
target_link_libraries(${PROJECT_NAME} PRIVATE raylib raygui_interface)
