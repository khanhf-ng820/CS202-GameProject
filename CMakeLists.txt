set(ENV{CMAKE_POLICY_VERSION_MINIMUM} "3.5")
set(CMAKE_POLICY_VERSION_MINIMUM "3.5" CACHE STRING "Force minimum policy version" FORCE)
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()
cmake_minimum_required(VERSION 3.20)
project(PvZGame LANGUAGES CXX)

# Force the modern C++20 Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ensure executable is placed directly in build/ (build/PvZGame.exe) across all generators
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}")

# Include CMake's FetchContent module to automate dependency retrieval
include(FetchContent)

# ---------------------------------------------------------
# Asset Package Retrieval (Automatic Asset Download)
# ---------------------------------------------------------
set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(ASSETS_URL "https://github.com/khanhf-ng820/CS202-GameProject/releases/download/v1.0-assets/assets.zip")

if(NOT EXISTS "${ASSETS_DIR}")
    message(STATUS "==========================================================")
    message(STATUS "Assets folder not found. Downloading game assets package...")
    message(STATUS "URL: ${ASSETS_URL}")
    message(STATUS "==========================================================")

    set(TEMP_ASSETS_ZIP "${CMAKE_BINARY_DIR}/assets_temp.zip")

    # 1. Download the assets zip with progress display
    file(DOWNLOAD 
        "${ASSETS_URL}" 
        "${TEMP_ASSETS_ZIP}"
        SHOW_PROGRESS
        STATUS DOWNLOAD_STATUS
        TLS_VERIFY ON
    )

    list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
    list(GET DOWNLOAD_STATUS 1 ERROR_MSG)

    if(NOT STATUS_CODE EQUAL 0)
        message(FATAL_ERROR "Failed to download assets: ${ERROR_MSG}\nPlease ensure you have an active internet connection or manually extract assets into ${ASSETS_DIR}.")
    endif()

    # 2. Extract into the project root directory
    message(STATUS "Extracting game assets into project source directory...")
    file(ARCHIVE_EXTRACT
        INPUT "${TEMP_ASSETS_ZIP}"
        DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}"
    )

    # 3. Clean up the temporary zip
    file(REMOVE "${TEMP_ASSETS_ZIP}")
    message(STATUS "Game assets successfully installed!")
endif()

# ---------------------------------------------------------
# Dependency: raylib
# ---------------------------------------------------------
set(BUILD_EXAMPLES OFF CACHE BOOL "Disable raylib examples" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "Disable raylib games" FORCE)
set(CUSTOMIZE_BUILD ON CACHE BOOL "Enable custom raylib config" FORCE)
set(SUPPORT_FILEFORMAT_JPG ON CACHE BOOL "Enable JPEG loading support" FORCE)

FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# ---------------------------------------------------------
# Dependency: raygui (Immediate Mode GUI Companion Tool)
# ---------------------------------------------------------
FetchContent_Declare(
    raygui
    URL https://github.com/raysan5/raygui/archive/refs/tags/4.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# ---------------------------------------------------------
# Dependency: nlohmann_json (Data Serialization Engine)
# ---------------------------------------------------------
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# Download and load all libraries into the active build environment
FetchContent_MakeAvailable(raylib raygui json)


# ---------------------------------------------------------
# Target Configuration
# ---------------------------------------------------------
# Locate all source files inside the src directory
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)

# Define the primary game executable target
add_executable(${PROJECT_NAME} ${SRC_FILES}
    include/entities/items/particle.h)
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/core
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/include/level
    ${CMAKE_CURRENT_SOURCE_DIR}/include/entities/plants
    ${CMAKE_CURRENT_SOURCE_DIR}/include/entities/zombies
    ${CMAKE_CURRENT_SOURCE_DIR}/include/entities/items
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
)

# raygui is a header-only library; wrap its include directory as an interface target
add_library(raygui_interface INTERFACE)
target_include_directories(raygui_interface INTERFACE ${raygui_SOURCE_DIR}/src)

# Link dependencies to your game target
# Note: nlohmann_json exports the namespaced interface target 'nlohmann_json::nlohmann_json'
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        raylib 
        raygui_interface
        nlohmann_json::nlohmann_json
)
