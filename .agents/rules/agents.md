---
trigger: always_on
description: Core project rules, build quirks, and architecture for the PvZ codebase
---

# Agent Instructions — Plants vs. Zombies (CS202 OOP Course)

This codebase uses C++20 and Raylib. Keep your edits concise, and follow these project-specific quirks to avoid critical bugs and build failures.

---

## 🛠 Build & Toolchain Quirks

*   **Do not just run `cmake --build`.** Since `CMakeLists.txt` globs sources recursively (`file(GLOB_RECURSE SRC_FILES src/*.cpp)`), adding a new `.cpp` file is **not** detected by a simple rebuild. You must run the full configure step:
    ```bash
    bash build.sh
    ```
    Or manually:
    ```bash
    cmake -S . -B build -DCMAKE_POLICY_VERSION_MINIMUM=3.5 && cmake --build build --config Release --parallel 4
    ```
*   **The `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` flag is mandatory.** Recent CMake (4.0+) drops compatibility with very old minimum version declarations in raylib/raygui. Without this shim, dependency configuration fails.
*   **For fast incremental compiles** (when no new files were added), you can run `bash remake.sh` (or `cmake --build build --config Release --parallel 4`).
*   **raygui is header-only.** There is no compiled `raygui` target. Do not link against `raygui`; link against the wrapped interface target **`raygui_interface`**.
*   **nlohmann_json** exports the namespaced target `nlohmann_json::nlohmann_json`. Use `#include <nlohmann/json.hpp>` directly.

---

## 📁 Directory & Module Structure

*   **Modular Subdirectories:** Source and header files are organized by domain under `src/` and `include/`:
    *   `core/` — Core infrastructure, resource loading, animation engine, font handling (`resources`, `AudioManager`, `BitmapFont`, `Reanimation`).
    *   `entities/` — In-game objects, subdivided into `plants/`, `zombies/`, and `items/` (`Projectile`, `SunItem`).
    *   `level/` — Gameplay loop, grid management, waves, and collisions (`Level1`).
    *   `ui/` — Menus and interface elements (`MainMenu`, `OptionsMenu`, `ShopMenu`, `SeedBank`, `SeedPacket`).
    *   `utils/` — Test harness and utilities (`testing`).
*   **Flat Header Inclusion:** `CMakeLists.txt` registers all subdirectories directly in the target include path. Always `#include` headers directly by filename without subfolder prefixes (e.g., `#include "PeaShooter.h"`, `#include "AudioManager.h"`).

---

## 📂 Asset Paths & Resolution

*   **NEVER hardcode asset paths.** The relative working directory of the compiled executable varies depending on the generator and config (`build/`, `build/Debug/`, etc.).
*   **Always resolve paths using `Resources::GetAssetPath`:**
    ```cpp
    res.GetAssetPath("assets/reanim/PeaShooter.reanim")
    ```
    This function checks multiple parent directory steps (`../`, `../../`, etc.) automatically.

---

## 🧟 Entity Scaffold & Coordinates

*   **Coordinate Types Differ:**
    *   `Plant::m_x` and `m_y` are **`int`**.
    *   `Zombie::m_x` and `m_y` are **`float`**.
    *   Passing one into the other's constructor causes silent truncation instead of a compiler warning.
*   **Debounce Shooting Pattern:** If a plant fires on frame `N` of its shooting animation, you MUST clear `did_shoot` on exactly frame `N+1` to reset the debounce:
    ```cpp
    if (m_anim.GetCurrentAnimName() == "anim_shooting" && m_anim.GetCurrentFrame() == 59 && !did_shoot) {
        // Fire projectile...
        did_shoot = true;
    }
    if (m_anim.GetCurrentFrame() == 60) {
        did_shoot = false; // Reset debounce
    }
    ```
*   **Standard Scale:** Always draw plants and zombies at scale **`1.6f`** (except `Chomper` which uses `1.4f`).

---

## 🎨 Reanim Animation Format (.reanim)

*   **The XML parser is ultra-simple:** `Resources::LoadReanim` parses line-by-line via basic substring searches. It has zero tolerance for multi-line tags, attributes, or structural formatting changes.
*   **Global Frame Index:** Every `<t>` is a *global* keyframe index shared by all tracks in the file.
*   **Inheritance:** Missing tags on a keyframe inherit their value from the previous frame (not default).
*   **Track Lengths:** Ensure every track has the same total number of `<t>` entries. Shorter tracks will silently stop drawing, making sprite parts vanish midway through.

---

## 🔊 Raylib Audio

*   **Centralized AudioManager:** Do not manage raw raylib music streams directly or call `UpdateMusicStream` in isolated components. Use the centralized singleton **`AudioManager::GetInstance()`** (in `include/core/AudioManager.h`) to play tracks (`PlayMusic(MusicTrack::...)`), stop music, and adjust volume. Its streaming update loop is already invoked every frame inside `main.cpp`.

---

## 📑 Rubric Requirements & Design Patterns

*   **Game State & Architecture:** Game state is managed via `MainMenu`, `OptionsMenu`, `ShopMenu` (Crazy Dave's Shop), and `Level1` (which handles the 5x9 grid, waves, economy via `SunItem`, and collisions). **Build on top of these existing systems** rather than reinventing them.
*   **"Multiple Players" interpretation:** This is a planned feature for a **pre-level Seed Deck Loadout Screen**, where the player builds their active `currentDeck` from `unlockedPlants` before a level. (Currently, `SeedBank` provides a default static deck). Mention this interpretation explicitly in the design document.
*   **OOP Design Patterns Tracker:** The rubric grades "Effective use of 5 design patterns" (25/100 points). Before adding subsystems, review `.agents/skills/design-patterns-tracker/SKILL.md` to map them onto needed patterns. Keep the status table updated in your commits.

---

## 🎳 Wall-nut Bowling Level (Conveyor Belt UI & Spawning)

*   **Backdrop Slot Geometry:** In the Wall-nut Bowling level, `ConveyorBelt_backdrop.png` spans 516px width with an inner conveyor slot running from `x = 8.0f` to `x = 507.0f`. Unlike `SeedBank.png`, it contains no sun counter box on the left. Set `leftMinX = 9.0f` for Card 0 alignment.
*   **Capacity-Gated Spawning:** Always gate conveyor belt card spawning in Wall-nut Bowling by `m_cards.empty() || m_cards.back().x < spawnX`. Pause the spawn timer when full (10 cards max) to prevent stationary card pile-up and overlap at the right spawn position (`spawnX = 459.0f`).

