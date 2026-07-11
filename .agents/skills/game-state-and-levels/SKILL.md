---
name: game-state-and-levels
description: Use when building level progression, wave-based zombie spawning, win/lose conditions, pausing, the main menu, or save/load functionality for this project. This covers the rubric's "3 Level Completion" (15 pts) and "Game State Management" requirements, which currently do not exist at all — main.cpp is a single perpetual loop with no concept of levels, waves, or win/loss. Always check this before adding anything that resembles a game phase, a spawner, or a save file.
---

# Game State and Levels

## Current state (as of the reanim-viewer version of main.cpp)

`main.cpp` currently just lets you preview one `Plant` and one `Zombie` at a
time via UI buttons — there is no lawn grid, no wave spawner, no win/lose
condition, no pause, and no save/load. Building these is required for the
rubric's "3 Level Completion" (15 pts) item and the broader "Game State
Management" requirement (start/pause/end, score/lives, save/load).

## Recommended state machine

```cpp
enum class GameState {
    MainMenu,
    SeedSelect,   // see the seed-deck-loadout skill — runs once per level
    Playing,
    Paused,
    LevelWon,
    LevelLost,
    GameOver
};
```

Own this in a singleton `GameManager` (mirror `Resources::GetInstance()`'s
Meyer's-singleton shape exactly) rather than as loose variables in
`main.cpp`. This also gives the `design-patterns-tracker` skill's State
pattern entry something concrete to point at — `Jalapeno`'s
`JalapenoState` enum in `include/Jalapeno.h` is a good small-scale reference
for the enum-driven-branch style already used in this codebase; follow the
same shape at the whole-game scale rather than introducing a different
state-management idiom.

## Levels and waves

```cpp
struct WaveSpec {
    float spawnTime;   // seconds since level start
    int zombieType;    // matches whatever enum/int scheme the eventual
                        // ZombieFactory uses (see design-patterns-tracker)
    int lane;           // which lawn row — see grid-collision-economy
};

struct LevelSpec {
    int number;
    std::vector<WaveSpec> waves;
    std::vector<std::string> unlockablePlant;  // plant(s) newly available
                                                 // after completing this level
};
```

- Increase difficulty across the 3 required levels by increasing wave
  count/density and using tougher zombie types (`FlagZombie` and
  `ZombieNormal` already exist as two difficulty tiers — add at least one
  more type for level 3 rather than just spawning more of the same two).
- Win condition: all waves for the level have been spawned **and** no
  zombie with `!isDead()` remains. `Zombie::isDead()` is already public.
- Lose condition: any live zombie's `getX()` (already a public getter on
  `Zombie`) drops below the house's x-boundary — check this every frame
  while `GameState::Playing`.
- Track "lives" (a small number of lawnmower-style saves, if you implement
  that mechanic) or just make any single zombie reaching the house an
  instant loss — either is acceptable for the rubric, but decide explicitly
  and document the choice in the design doc rather than leaving it implicit.

## Save/load

`CMakeLists.txt` already fetches `nlohmann_json` and links
`nlohmann_json::nlohmann_json` — this dependency is currently unused
anywhere in the codebase, which is a strong signal it was pulled in
specifically for this feature. Use `#include <nlohmann/json.hpp>` directly;
no CMake changes are needed.

Suggested save schema (keep field names stable once `seed-deck-loadout` also
needs to read/write this same file):

```json
{
  "currentLevel": 2,
  "sunCount": 150,
  "unlockedPlants": ["PeaShooter", "SnowPea", "Repeater"],
  "currentDeck": ["PeaShooter", "SnowPea"],
  "plantedGrid": [
    {"row": 1, "col": 3, "type": "PeaShooter", "hp": 300}
  ]
}
```

- `unlockedPlants` grows permanently as levels are completed.
- `currentDeck` is chosen fresh each level via the seed-select screen (see
  `seed-deck-loadout`) but persisting the *last* chosen deck as a default
  for next time is a reasonable nice-to-have.
- Write the save file through a single `GameManager::SaveGame()` /
  `LoadGame()` pair rather than scattering `nlohmann::json` calls across
  multiple files — keeps the schema in one place.

## Don't forget

- Pausing should stop `m_anim.Update(deltaTime)` calls and wave-spawn timers
  but should **not** stop rendering — `Reanimation::SetPaused`/`IsPaused`/
  `TogglePause` already exist on `Reanimation` and are unused anywhere in
  the codebase; wire the global pause state to call `TogglePause()` on the
  active plants'/zombies' `Reanimation` instances, or add a game-level time
  scale of 0 instead if you'd rather not touch every entity.
- Update `design-patterns-tracker`'s table when this lands — a second
  Singleton (`GameManager` alongside `Resources`) is fine, and if you
  formalize the `GameState` enum into real State objects instead of an
  enum-driven branch, that upgrades the State pattern entry from "Partial"
  to "Done."
