# CS202-GameProject
A "Plants Vs. Zombies"-inspired game for CS202 - Programming Systems course.

***Disclaimer:*** This repository is a non-commercial, educational project created solely for learning and teaching purposes. It is not affiliated with, authorized, or endorsed by *PopCap Games* or *Electronic Arts*. All trademarks and registered trademarks belong to their respective owners. Content is used here under Fair Use guidelines for educational analysis and instruction. This project does not aim to infringe on any copyrights and is not intended for distribution or sale. No financial profit is being made from this project. If you are the copyright holder and have concerns about this use, please contact us to address them promptly.

<!-- ## Agents and Skills -->
<!---->
<!-- ### Keeping these current -->
<!---->
<!-- Two of these skills are living documents meant to be edited as the project -->
<!-- evolves, not just read: -->
<!---->
<!-- - `.agents/skills/design-patterns-tracker/SKILL.md` — its status table should be updated -->
<!--   in the same commit as any change to a design pattern's implementation. -->
<!-- - `.agents/skills/game-state-and-levels/SKILL.md`'s save-file JSON schema and -->
<!--   `seed-deck-loadout/SKILL.md` should stay in sync on field names — check -->
<!--   one before editing the other. -->
<!---->
<!-- ### Open decision already made -->
<!---->
<!-- `.agents/skills/seed-deck-loadout` documents the chosen interpretation of the rubric's -->
<!-- "Multiple Players" item (a pre-level plant-loadout screen) — see that -->
<!-- skill's "Why this exists" section for the reasoning, and make sure the -->
<!-- final design doc states it explicitly rather than leaving a grader to -->
<!-- infer it. -->

---

## Codebase Architecture and Structure

This repository is a C++20 implementation of a Plants vs. Zombies clone built using the Raylib multimedia framework. It incorporates a custom parsing and rendering engine for PopCap's `.reanim` XML keyframe animation format.

### Directory Layout and File Overview

```
CS202-PlantsVsZombies/
├── assets/                  # Game assets (sprites, reanim XML definitions, fonts, particles)
│   ├── reanim/              # PopCap .reanim keyframe animation XMLs (PeaShooter, Zombie, etc.)
│   ├── images/              # Static UI textures, background lawn, seed packets
│   ├── fonts/               # Custom bitmap font definitions (DwarvenTodcraft24)
│   └── particles/           # Particle effect textures
├── include/                 # C++ Header files
│   ├── BitmapFont.h          # Custom bitmap font renderer for PopCap UI typography
│   ├── BucketheadZombie.h    # Buckethead Zombie subclass (armor extension)
│   ├── CherryBomb.h          # Cherry Bomb plant (area-of-effect explosive)
│   ├── Chomper.h             # Chomper plant (melee bite and chew mechanics)
│   ├── ConeheadZombie.h      # Conehead Zombie subclass (armor extension)
│   ├── Cornpult.h            # Cornpult plant (lobbed parabolic trajectory)
│   ├── FirePea.h             # Fire Pea shooter plant (flaming pea projectile)
│   ├── FlagZombie.h          # Flag Zombie subclass (wave leader)
│   ├── GatlingPea.h          # Gatling Pea shooter plant (quad-pea barrage)
│   ├── Jalapeno.h            # Jalapeno plant (lane-clearing explosive line)
│   ├── MainMenu.h           # PopCap interactive main menu (SelectorScreen.reanim)
│   ├── Melonpult.h           # Melonpult plant (heavy lobbed splash projectile)
│   ├── OptionsMenu.h         # Game settings and options overlay panel
│   ├── PeaShooter.h          # Base Pea Shooter plant (straight projectile)
│   ├── Plant.h               # Abstract base class for all Plant entities
│   ├── Projectile.h         # Projectile class handling flying peas and lobbed melons
│   ├── Reanimation.h        # PopCap .reanim keyframe animation player and track manager
│   ├── reanim.h             # Struct definitions for .reanim XML keyframes and tracks
│   ├── Repeater.h            # Repeater plant (double pea projectile)
│   ├── resources.h           # Centralized resource and asset management (Singleton)
│   ├── SnowPea.h             # Snow Pea plant (freezing pea projectile)
│   ├── SunFlower.h           # Sunflower plant (sun generation timer)
│   ├── SunItem.h             # Sun item drop (falling and click collection)
│   ├── UIHelpers.h           # Raylib interactive UI components (buttons, resolution scaling)
│   ├── Wallnut.h             # Wall-nut plant (defensive high-HP shield)
│   ├── Zombie.h              # Abstract base class for all Zombie entities
│   └── ZombieNormal.h        # Standard Zombie subclass
├── src/                     # C++ Source implementation files
├── build.sh                 # CMake configure and Release build automation script
├── remake.sh                # Incremental make script for fast rebuilds
└── CMakeLists.txt           # Build configuration pinning Raylib, raygui, nlohmann_json
```

---

## Core Systems and Technical Architecture

### 1. PopCap Reanim Keyframe Engine

The animation pipeline (`reanim.h`, `Reanimation.h`, `Reanimation.cpp`) reads PopCap's XML animation format and renders multi-track keyframed characters in real time.

* **Keyframe Parsing and Property Inheritance**: The parser processes track keyframes (`<t>`) line by line. Keyframe properties (position, scale, skew, opacity, image swaps) inherit values from preceding keyframes when not explicitly defined in the current keyframe tag.
* **Hidden Frame Sentinel**: Keyframes marked with `<f>-1</f>` signal that the corresponding track should be hidden during playback.
* **Dual-Layer Animation Blending**: The engine supports simultaneous playback of a base track sequence (such as a lower-body walking loop) and an upper-body overlay sequence (such as a shooting or biting action).
* **2D Affine Transformation Matrix**: For each active track at frame $t$, the system computes an affine transformation matrix combining scale ($s_x, s_y$) and skew/rotation ($k_x, k_y$):

$$\begin{bmatrix} m_{00} & m_{10} \\ m_{01} & m_{11} \end{bmatrix} = \begin{bmatrix} s_x \cos(k_x) & s_x \sin(k_x) \\ -s_y \sin(k_y) & s_y \cos(k_y) \end{bmatrix}$$

* **OpenGL Batch Rendering**: Drawing is executed by pushing track matrices directly to Raylib's internal matrix stack using `rlPushMatrix()`, `rlMultMatrixf()`, `rlDrawRenderBatchActive()`, and `rlPopMatrix()`.
* **Frame-Exact Debounce Pattern**: Entity firing logic relies on keyframe index checks. When a plant triggers a projectile spawn on frame $N$, it sets a internal `did_shoot` flag, which is cleared on frame $N+1$ to prevent duplicate instantiations within a single animation pass.

### 2. Zombie Physics and Limb Detachment

The `Zombie` base class (`Zombie.h`) integrates a `FallingPart` physics structure to handle limb and armor detachment upon taking critical damage:

* **Particle Physics**: `FallingPart` tracks detached limbs (heads, arms) or armor pieces (cones, buckets) with linear velocity ($v_x, v_y$), angular rotation speed, position coordinates, and lifespan decay timers.
* **Health Threshold Triggers**: Subclasses like `ZombieNormal`, `ConeheadZombie`, and `BucketheadZombie` override `takeDamage()` to swap track visibility, detach armor, or spawn falling head particles when health drops below specific thresholds.

### 3. Resource Management and Asset Resolution

Asset loading is centralized within the `Resources` class (`resources.h`, `resources.cpp`):

* **Path Resolution**: `Resources::GetAssetPath()` handles relative directory traversal automatically (`./`, `../`, `../../`, etc.), ensuring asset paths resolve regardless of whether the executable is launched from the project root or build subdirectories.
* **Pixel-Perfect Alpha Hit Testing**: `Resources::IsPixelTransparent()` queries CPU-side image buffers (`Image`) to perform pixel-accurate mouse hit detection on non-rectangular UI elements, such as the main menu tombstone buttons.

### 4. Custom UI and Typography Engine

* **Bitmap Font Rendering**: `BitmapFont` (`BitmapFont.h`, `BitmapFont.cpp`) parses PopCap font definition files alongside PNG texture atlases, supporting custom kerning pairs, glyph advance widths, and character offsets.
* **Virtual Input Resolution Scaling**: `UIHelpers` (`UIHelpers.h`, `UIHelpers.cpp`) provides `SetVirtualMouseScale()` and `GetVirtualMousePosition()`, ensuring mouse coordinates map accurately to the fixed 800x600 virtual canvas regardless of display stretching.
* **Modal Overlay Input Locking**: `SetUIInteractionEnabled()` allows modal menus (such as `OptionsMenu`) to freeze underlying UI interactions when active.

### 5. Main Loop and Rendering Pipeline

The main execution loop in `src/main.cpp` follows a four-stage pipeline:

1. **Virtual Mouse Scaling**: Calculates scaling ratios based on current window dimensions relative to the target 800x600 render target.
2. **State Updates**: Evaluates logic for the active state (`MainMenu`, `OptionsMenu`, or `PlantTest`).
3. **Offscreen Canvas Pass**: Renders lawn backgrounds, plants, zombies, projectiles, sun items, and UI panels into an 800x600 `RenderTexture2D` canvas using `BeginTextureMode()`.
4. **Window Presentation Pass**: Draws the canvas texture to the window using `DrawTexturePro()` with hardware bilinear scaling.

---

## Plan and Design of 5 OOP Design Patterns

To meet project course requirements (25/100 points) and ensure a modular codebase, five Object-Oriented Design Patterns are incorporated into the software design:

```
[1. Singleton Pattern] -----> Resources Asset Manager
[2. Factory Pattern]   -----> Plant and Zombie Spawners
[3. State Pattern]     -----> Game State Engine and Zombie AI
[4. Observer Pattern]  -----> Event Dispatcher and HUD/Audio Subsystems
[5. Strategy Pattern]  -----> Projectile Trajectories and Movement Algorithms
```

---

### 1. Singleton Pattern (Creational)

* **Current Status**: Implemented in `Resources::GetInstance()` (`include/resources.h`).
* **Design**: Implemented as a Meyer's Singleton using a static local instance within `GetInstance()`. Constructors are private, and copy operations are deleted.
* **Rationale**: Prevents redundant loading of heavy textures, font atlases, and `.reanim` definitions into GPU memory.
* **Extension Plan**: Extend this pattern to create a centralized `AudioManager` for sound effect dispatch and a `GameConfig` manager for persistent user settings.

---

### 2. Factory Method Pattern (Creational)

* **Current Status**: Planned refactoring for entity creation in `main.cpp`.
* **Design**: Dedicated factory classes (`PlantFactory` and `ZombieFactory`) exposing static creation interfaces:
  * `PlantFactory::CreatePlant(PlantType type, int x, int y)`
  * `ZombieFactory::CreateZombie(ZombieType type, float x, float y)`
* **Rationale**: Replaces manual `if-else` construction chains in `main.cpp` with a clean creation interface, ensuring adding new plant or zombie types does not require modifying UI or game loop code.
* **Implementation Blueprint**:

```cpp
enum class PlantType { PeaShooter, SnowPea, SunFlower, Wallnut, CherryBomb, Chomper, Jalapeno, Repeater, GatlingPea, Cornpult, Melonpult, FirePea };

class PlantFactory {
public:
    static std::unique_ptr<Plant> CreatePlant(PlantType type, int x, int y) {
        Resources& res = Resources::GetInstance();
        switch (type) {
            case PlantType::PeaShooter: return std::make_unique<PeaShooter>(res, x, y);
            case PlantType::SnowPea:    return std::make_unique<SnowPea>(res, x, y);
            case PlantType::SunFlower:  return std::make_unique<SunFlower>(res, x, y);
            case PlantType::Wallnut:    return std::make_unique<Wallnut>(res, x, y);
            case PlantType::CherryBomb: return std::make_unique<CherryBomb>(res, x, y);
            case PlantType::Chomper:    return std::make_unique<Chomper>(res, x, y);
            case PlantType::Jalapeno:   return std::make_unique<Jalapeno>(res, x, y);
            case PlantType::Cornpult:   return std::make_unique<Cornpult>(res, x, y);
            case PlantType::Melonpult:  return std::make_unique<Melonpult>(res, x, y);
            case PlantType::FirePea:    return std::make_unique<FirePea>(res, x, y);
            default:                    return nullptr;
        }
    }
};
```

---

### 3. State Pattern (Behavioral)

* **Current Status**: Partially implemented via basic enums (`GameState` in `main.cpp` and `JalapenoState` in `Jalapeno.h`). Planned for expansion into class-based state objects.
* **Design**: Abstract `IGameState` interface defining `Enter()`, `Update()`, `Draw()`, and `Exit()` lifecycle methods, managed by a `GameStateManager` context.
* **Rationale**: Encapsulates distinct game phases (`MainMenuState`, `SeedSelectState` for pre-level plant deck selection, `PlayingState`, `PauseState`, `GameOverState`) and entity behavior states (`WalkingState`, `EatingState`, `DyingState`).
* **Implementation Blueprint**:

```cpp
class IGameState {
public:
    virtual ~IGameState() = default;
    virtual void Enter() = 0;
    virtual void Update(float dt) = 0;
    virtual void Draw() = 0;
    virtual void Exit() = 0;
};

class GameStateManager {
private:
    std::unique_ptr<IGameState> m_currentState;
public:
    void ChangeState(std::unique_ptr<IGameState> newState) {
        if (m_currentState) m_currentState->Exit();
        m_currentState = std::move(newState);
        if (m_currentState) m_currentState->Enter();
    }
    void Update(float dt) { if (m_currentState) m_currentState->Update(dt); }
    void Draw() { if (m_currentState) m_currentState->Draw(); }
};
```

---

### 4. Observer Pattern (Behavioral)

* **Current Status**: Planned.
* **Design**: Centralized `EventManager` working with an `IObserver` notification interface.
* **Rationale**: Decouples gameplay events from secondary systems. When a sun is collected, a zombie dies, or a plant is destroyed, `EventManager` notifies listening subsystems (HUD, score tracker, audio engine, lawn grid) without hard coupling.
* **Implementation Blueprint**:

```cpp
enum class GameEvent { SunCollected, ZombieKilled, PlantDestroyed, LawnmowerTriggered };

struct EventData { int value; int gridX; int gridY; };

class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void OnNotify(GameEvent event, const EventData& data) = 0;
};

class EventManager {
private:
    std::unordered_map<GameEvent, std::vector<IObserver*>> m_listeners;
public:
    static EventManager& GetInstance() {
        static EventManager instance;
        return instance;
    }
    void Subscribe(GameEvent event, IObserver* observer) {
        m_listeners[event].push_back(observer);
    }
    void Notify(GameEvent event, const EventData& data) {
        for (auto* obs : m_listeners[event]) obs->OnNotify(event, data);
    }
};
```

---

### 5. Strategy Pattern (Behavioral)

* **Current Status**: Planned refactoring for `Projectile` trajectory handling (`include/Projectile.h`).
* **Design**: `ITrajectoryStrategy` interface encapsulating projectile movement mathematics away from entity rendering logic.
* **Rationale**: Replaces boolean flag checks (`m_isLobbed`) with interchangeable strategy algorithms for linear pea travel (`StraightTrajectoryStrategy`), parabolic catapult arcs (`LobbedTrajectoryStrategy`), and instant radial explosions (`AoeExplosionStrategy`).
* **Implementation Blueprint**:

```cpp
class ITrajectoryStrategy {
public:
    virtual ~ITrajectoryStrategy() = default;
    virtual Vector2 CalculatePosition(Vector2 start, Vector2 target, float speed, float t) = 0;
    virtual bool HasHitTarget(Vector2 current, Vector2 target) = 0;
};

class StraightTrajectoryStrategy : public ITrajectoryStrategy {
public:
    Vector2 CalculatePosition(Vector2 start, Vector2 target, float speed, float t) override {
        return { start.x + speed * t, start.y };
    }
    bool HasHitTarget(Vector2 current, Vector2 target) override {
        return current.x >= 800.0f;
    }
};

class LobbedTrajectoryStrategy : public ITrajectoryStrategy {
private:
    float m_maxHeight = 120.0f;
public:
    Vector2 CalculatePosition(Vector2 start, Vector2 target, float speed, float t) override {
        float totalDist = target.x - start.x;
        float currDist = speed * t;
        float progress = (totalDist > 0) ? (currDist / totalDist) : 1.0f;
        float currentY = start.y - (4.0f * m_maxHeight * progress * (1.0f - progress));
        return { start.x + currDist, currentY };
    }
    bool HasHitTarget(Vector2 current, Vector2 target) override {
        return current.x >= target.x;
    }
};
```

---

## Build and Compilation Instructions

### Prerequisites

* CMake 3.5 or higher
* C++20 compliant compiler (GCC, Clang, or MSVC)
* Raylib dependencies (OpenGL graphics drivers)

### Building the Project

```bash
# Run the automated build script (applies mandatory CMAKE_POLICY_VERSION_MINIMUM shim)
bash build.sh

# Run executable
./build/PvZGame
```

