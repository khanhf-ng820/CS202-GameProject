# Agent Instructions — Plants vs. Zombies (CS202)

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
*   **For fast incremental compiles** (when no new files were added), you can run `bash remake.sh` (or `make -j4` directly inside `build/`).
*   **raygui is header-only.** There is no compiled `raygui` target. Do not link against `raygui`; link against the wrapped interface target **`raygui_interface`**.
*   **nlohmann_json** exports the namespaced target `nlohmann_json::nlohmann_json`. Use `#include <nlohmann/json.hpp>` directly.

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

*   **BGM update loop:** Raylib background music requires `UpdateMusicStream(bgMusic)` to be called **every frame** in the main loop. Forgetting this causes the music to play for a fraction of a second and silently stop.

---

## 📑 Rubric Requirements & Design Patterns

*   **"Multiple Players" interpretation:** This is implemented as a **pre-level Seed Deck Loadout Screen** (`GameState::SeedSelect`), where the player builds their active `currentDeck` from `unlockedPlants`. Mention this interpretation explicitly in the design document.
*   **Design Patterns Tracker:** The rubric grades "Effective use of 5 design patterns" (25/100 points). Before adding subsystems, review `.agents/skills/design-patterns-tracker/SKILL.md` to map them onto needed patterns. Keep the status table updated in your commits.
