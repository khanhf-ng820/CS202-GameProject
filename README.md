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

This project is a C++20 implementation of a Plants vs. Zombies clone engineered using the Raylib multimedia framework. It incorporates a custom parsing, transformation, and rendering engine for PopCap's proprietary `.reanim` XML keyframe animation format, along with custom bitmap font typography, multi-user profile persistence, progressive asset loading, and wave-based level progression.

### Directory Layout and Module Organization

The codebase is organized into five modular subdirectories under `include/` and `src/`:

```
CS202-PlantsVsZombies/
│   ├── reanim/                      # PopCap .reanim keyframe animation XML definitions
│   ├── images/                      # Texture atlases, background lawn art, seed cards
│   ├── fonts/                       # Custom bitmap font descriptors and PNG atlases
│   ├── particles/                   # Particle effect textures and overlays
│   └── PlantSeedPackets/            # Seed packet UI graphics for all plant types
├── include/                         # Header files (.h)
│   ├── core/                        # Core runtime infrastructure and singletons
│   │   ├── AudioManager.h           # Centralized music streaming and SFX management
│   │   ├── BitmapFont.h             # PopCap bitmap font descriptor parser and renderer
│   │   ├── ProfileManager.h         # Multi-user profile persistence and save management
│   │   ├── Reanimation.h            # Skeletal animation controller and matrix compositor
│   │   ├── reanim.h                 # Struct definitions for keyframes, tracks, and ranges
│   │   └── resources.h              # Centralized asset manager (Meyer's Singleton)
│   ├── entities/                    # Game objects and entity hierarchies
│   │   ├── items/                   # Interactive items, projectiles, and environmental objects
│   │   │   ├── LawnMower.h          # Emergency row-clearing lawn mower entity
│   │   │   ├── Projectile.h         # Linear and lobbed trajectory projectile entity
│   │   │   ├── SunItem.h            # Sun currency entity with bezier collection physics
│   │   │   ├── Vase.h               # Breakable mystery vase entity for Vasebreaker mode
│   │   │   └── particle.h           # Particle emitter and burst effect structures
│   │   ├── plants/                  # Plant base class and all 20+ plant subclasses
│   │   │   ├── Plant.h              # Abstract base class for all plant entities
│   │   │   ├── PeaShooter.h         # Basic horizontal single-pea shooter
│   │   │   ├── SnowPea.h            # Freezing projectile shooter with slow debuff
│   │   │   ├── Repeater.h           # Rapid two-pea burst shooter
│   │   │   ├── GatlingPea.h         # Quad-pea barrage heavy shooter
│   │   │   ├── FirePea.h            # Double-damage flaming pea shooter
│   │   │   ├── SunFlower.h          # Sun generation economic unit
│   │   │   ├── Wallnut.h            # High-durability defensive shield with visual damage states
│   │   │   ├── CherryBomb.h         # 3x3 area-of-effect lethal explosive
│   │   │   ├── Jalapeno.h           # State-driven full-lane incinerator
│   │   │   ├── Chomper.h            # Melee devourer with extended chewing cooldown
│   │   │   ├── PotatoMine.h         # Underground contact explosive with arming delay
│   │   │   ├── Squash.h             # Proximity-triggered smashing entity
│   │   │   ├── Garlic.h             # Defensive entity that diverts zombies to adjacent rows
│   │   │   ├── Torchwood.h          # Projectile modifier that ignites peas on pass-through
│   │   │   ├── SpikeRock.h          # Reinforced ground hazard that punctures tires and armor
│   │   │   ├── Caltrop.h            # Basic ground spikeweed hazard
│   │   │   ├── IceShroom.h          # Screen-wide freeze and zombie immobilization
│   │   │   ├── Gravebuster.h        # Grave consumption utility for night stages
│   │   │   ├── Cornpult.h           # Lobbed catapult firing corn and stunning butter
│   │   │   ├── Cabbagepult.h        # Lobbed catapult launching heavy cabbages
│   │   │   ├── Melonpult.h          # Heavy lobbed catapult with splash damage radius
│   │   │   └── BowlingNut.h         # Physics-driven rolling Wall-nuts for minigame mode
│   │   └── zombies/                 # Zombie base class and all 7+ zombie subclasses
│   │       ├── Zombie.h             # Abstract base class with limb detachment physics
│   │       ├── ZombieNormal.h       # Standard browncoat zombie
│   │       ├── FlagZombie.h         # Fast wave leader carrying a flag
│   │       ├── ConeheadZombie.h     # Traffic cone armored zombie (+370 HP)
│   │       ├── BucketheadZombie.h   # Steel bucket heavy armored zombie (+1100 HP)
│   │       ├── FootballZombie.h     # High-speed athlete zombie with heavy helmet
│   │       ├── NewspaperZombie.h    # Shielded zombie with enraged dash transition
│   │       └── PoleVaultingZombie.h # Vaulting zombie with plant-jumping traversal
│   ├── level/                       # Level progression and minigame loops
│   │   ├── Level1.h                 # Adventure Level 1 base class and game loop
│   │   ├── Level2.h                 # Adventure Level 2 (Day stage with Coneheads)
│   │   ├── Level3.h                 # Adventure Level 3 (Day stage with multi-flags)
│   │   ├── Level4.h                 # Adventure Level 4 (Night stage with Gravestones)
│   │   ├── Level5.h                 # Adventure Level 5 (Night stage with grave risers)
│   │   ├── Level6.h                 # Adventure Level 6 (Night climax wave stage)
│   │   ├── BowlingLevel.h           # Wall-nut Bowling minigame with conveyor belt UI
│   │   └── VasebreakerLevel.h       # Vasebreaker puzzle mode with mallet interaction
│   ├── ui/                          # Menus, HUD components, and dialogs
│   │   ├── AlmanacMenu.h            # Suburban Almanac with live animated preview models
│   │   ├── HelpMenu.h               # Interactive help and instructions parchment
│   │   ├── InGameMenu.h             # In-game pause modal with options and restart
│   │   ├── LevelSelectMenu.h        # Stage progression tree (Day and Night chapters)
│   │   ├── LoadingScreen.h          # Dirt/grass progressive asset load bar
│   │   ├── MainMenu.h               # Interactive tombstone menu (SelectorScreen.reanim)
│   │   ├── OptionsMenu.h            # Settings overlay (volume sliders, resolution presets)
│   │   ├── QuizMenu.h               # Crazy Dave's trivia quiz with rewards
│   │   ├── SeedBank.h               # Top HUD bar displaying sun count and selected cards
│   │   ├── SeedPacket.h             # Seed card cooldown sweeps and drag-and-drop
│   │   ├── SeedSelectMenu.h         # Pre-level plant deck builder (Loadout screen)
│   │   ├── ShopMenu.h               # Crazy Dave's Twiddydinkies car trunk shop
│   │   ├── UIHelpers.h              # Virtual mouse scaling and immediate-mode buttons
│   │   └── UserDialog.h             # Player profile management dialog
│   └── utils/                       # Diagnostic and testing utilities
│       └── testing.h                # Zen Garden animation and hitbox visualizer
├── src/                             # C++ Source implementation files
│   ├── core/                        # Implementations for runtime singletons and parsers
│   ├── entities/                    # Implementations for plants, zombies, and items
│   ├── level/                       # Implementations for adventure levels and minigames
│   ├── ui/                          # Implementations for menus and interface elements
│   ├── utils/                       # Implementations for test harnesses
│   └── main.cpp                     # Application entry point and top-level render loop
├── build.sh                         # Full CMake configuration and release build script
├── remake.sh                        # Fast incremental compile script for existing files
└── CMakeLists.txt                   # Build specification with Raylib, raygui, and nlohmann_json
```

---

## Core Subsystems and Engine Architecture

### 1. PopCap Reanim Keyframe Animation Engine

The animation subsystem (`reanim.h`, `Reanimation.h`, `Reanimation.cpp`, `resources.h`) provides full real-time reconstruction of PopCap's XML skeletal animation format.

* **Keyframe XML Parsing and Value Inheritance**: The parser reads tracks and keyframes (`<t>`) sequentially. Properties not explicitly modified on a keyframe (such as coordinates $x, y$, scale $s_x, s_y$, rotation $k_x, k_y$, or texture bindings) inherit their values directly from the preceding frame ($N-1$).
* **Hidden Track Sentinels**: Tracks containing the `<f>-1</f>` frame property are hidden during rendering passes, allowing dynamic attachment and detachment of visual parts (such as hats, shields, or limbs).
* **2D Affine Transformation Matrix**: For each active track at keyframe $t$, the engine computes an affine transformation matrix combining scale ($s_x, s_y$) and rotation/skew ($k_x, k_y$):

$$\begin{bmatrix} m_{00} & m_{10} \\ m_{01} & m_{11} \end{bmatrix} = \begin{bmatrix} s_x \cos(k_x) & s_x \sin(k_x) \\ -s_y \sin(k_y) & s_y \cos(k_y) \end{bmatrix}$$

* **OpenGL Matrix Stack Pipeline**: Visual tracks are rendered using Raylib's low-level matrix stack: `rlPushMatrix()`, `rlMultMatrixf()`, `rlDrawRenderBatchActive()`, and `rlPopMatrix()`, ensuring hardware-accelerated composite drawing.
* **Dual-Layer Animation Blending**: Supports simultaneous playback of a base locomotion track (such as a zombie walking loop) blended with an independent upper-body overlay sequence (such as a biting or eating track).
* **Frame-Exact Debounce Pattern**: Entity firing actions check exact keyframe indices. When a plant triggers a projectile spawn on frame $N$, it sets an internal `did_shoot` flag, which is cleared on frame $N+1$ to prevent duplicate instantiations during the keyframe window.

### 2. Centralized Audio Management (`AudioManager`)

Audio playback is managed via a Meyer's Singleton (`include/core/AudioManager.h`, `src/core/AudioManager.cpp`) wrapping Raylib's audio streaming engine:

* **Music Streaming**: Handles smooth transitions between background tracks (`MusicTrack::MainMenu`, `DayStage`, `NightStage`, `Bowling`, `Vasebreaker`, `ZenGarden`, `CrazyDaveTheme`, `WinMusic`, `LoseMusic`).
* **Volume Attenuation**: Provides synchronized dynamic volume scaling for background music and concurrent multi-channel sound effects.
* **Global Stream Pumping**: Invokes `UpdateMusicStream()` once per frame in the main execution loop to prevent buffer underruns.

### 3. Player Profile and Persistence Engine (`ProfileManager`)

User progress is persisted across sessions via a dedicated Singleton (`include/core/ProfileManager.h`, `src/core/ProfileManager.cpp`):

* **JSON Serialization**: Stores player profiles (`UserProfile`) containing active coin balance, unlocked plant card catalogs, and maximum level completion records.
* **Multi-User Management**: Supports creating, renaming, deleting, and switching player profiles dynamically via `UserDialog`.
* **Deck Unlocking Progression**: Manages the default starter deck and validates plant card availability during the pre-level seed chooser phase.

### 4. Custom Typography and Bitmap Font Engine (`BitmapFont`)

PopCap UI typography is rendered through a custom bitmap font parser (`include/core/BitmapFont.h`, `src/core/BitmapFont.cpp`):

* **Descriptor Parser**: Reads XML and TXT font descriptor metrics paired with PNG texture atlases (`DwarvenTodcraft24`, `HouseofTerror28`, `BrianneTod16`, `ContinuumBold14`).
* **Packed Integer Kerning**: Uses a packed 16-bit integer map (`uint16_t` key constructed as `((c1 << 8) | c2)`) to eliminate dynamic heap allocations during per-character kerning lookups.
* **Text Layout Capabilities**: Supports centered text rendering, line height scaling, character offsets, and automated word wrapping within bounded rectangles.

### 5. Virtual Resolution and UI Engine (`UIHelpers`)

* **Fixed Virtual Canvas**: All game rendering occurs on a fixed 800x600 `RenderTexture2D` canvas, which is stretched to fit the physical window using `DrawTexturePro()` with hardware bilinear filtering.
* **Virtual Mouse Transformation**: `SetVirtualMouseScale()` maps actual window mouse coordinates to the 800x600 virtual canvas coordinates, ensuring hitboxes align across all display resolutions.
* **Pixel-Perfect Alpha Hit Testing**: `Resources::IsPixelTransparent()` inspects CPU-side image pixel buffers to perform exact hit-testing on irregular non-rectangular buttons (such as main menu tombstones).
* **Modal Input Locking**: `SetUIInteractionEnabled()` disables background UI hover and click processing when modal overlays (options, shop, help) are open.

---

## Entity Hierarchy and Game Mechanics

```
                         +-------------------------+
                         |      Resources          | (Singleton)
                         +-------------------------+
                                      |
              +-----------------------+-----------------------+
              |                       |                       |
              v                       v                       v
     +-----------------+     +-----------------+     +-----------------+
     |   Reanimation   |     |   BitmapFont    |     | ProfileManager  |
     +-----------------+     +-----------------+     +-----------------+

                                 +---------+
                                 |  Plant  | (Abstract Base)
                                 +----+----+
          +---------------+-----------+-----------+---------------+
          |               |           |           |               |
          v               v           v           v               v
     PeaShooter       SunFlower    Wallnut    CherryBomb       Jalapeno
     SnowPea          Torchwood    Garlic     PotatoMine       Chomper
     Repeater         Caltrop      SpikeRock  Squash           IceShroom
     GatlingPea       Cornpult     Melonpult  Cabbagepult      BowlingNut

                                 +---------+
                                 | Zombie  | (Abstract Base)
                                 +----+----+
          +---------------+-----------+-----------+---------------+
          |               |           |           |               |
          v               v           v           v               v
     ZombieNormal    FlagZombie  ConeheadZombie BucketheadZombie FootballZombie
                                 NewspaperZombie PoleVaultingZombie
```

### 1. Plant Taxonomy (`Plant.h`)

All plants derive from the abstract base class `Plant`, which defines integer grid positions ($m_x, m_y$), current and max health ($m_hp, m_maxHp$), sun cost, recharge timers, and pure virtual `update()` and `draw()` methods.

* **Linear Projectile Shooters**: `PeaShooter`, `SnowPea` (applies a chilling slow debuff), `Repeater` (two-shot burst), `GatlingPea` (four-shot barrage), and `FirePea` ($2\times$ damage).
* **Lobbed Catapults**: `Cabbagepult`, `Cornpult` (flings corn kernels and occasional butter that stuns zombies for 4 seconds), and `Melonpult` (launches heavy melons inflicting splash damage across adjacent tiles).
* **Economic Producers**: `SunFlower` (produces 25 sun currency every 24 seconds with a pulsing glow animation).
* **Defensive Shields**: `Wallnut` (4000 HP barrier that swaps texture tracks dynamically to display cracked and severely damaged states).
* **Tactical Disruptors**: `Garlic` (diverts zombies biting it to adjacent lanes), `Torchwood` (transforms normal peas into fire peas), `SpikeRock` and `Caltrop` (ground hazards that puncture zombie feet and vehicles), and `IceShroom` (temporarily immobilizes all zombies on screen).
* **Instant Area-of-Effect Explosives**: `CherryBomb` (detonates in a 3x3 grid radius dealing 1800 damage), `Jalapeno` (incinerates an entire horizontal row), `PotatoMine` (arms underground after 14 seconds and detonates on contact), and `Squash` (jumps and crushes approaching zombies).

### 2. Zombie Taxonomy (`Zombie.h`)

All zombies derive from the abstract base class `Zombie`, utilizing continuous float coordinates ($m_x, m_y$) for smooth movement, walk speeds, damage-per-tick values, eating states, and `FallingPart` particle physics for detached limbs.

* **Standard and Swarm Types**: `ZombieNormal` and `FlagZombie` (leads huge waves with increased walk speed).
* **Multi-Layer Armored Types**: `ConeheadZombie` (cone armor absorption) and `BucketheadZombie` (bucket armor absorption with visual dent stages). Armor breaks and pops off as physics particles before base body damage occurs.
* **Specialist Types**: `FootballZombie` (high-velocity athlete with heavy armor durability), `NewspaperZombie` (absorbs damage with a newspaper shield; when destroyed, enraged with double walk speed and attack rate), and `PoleVaultingZombie` (sprints and leaps over the first plant barrier before resuming a standard walk).

### 3. Projectiles, Currency, and Interactive Items

* **`Projectile`**: Encapsulates linear horizontal motion for peas and parabolic arc trajectories for catapults:
  $$y(t) = y_0 - 4h \cdot \left(\frac{x - x_0}{x_{\text{target}} - x_0}\right) \left(1 - \frac{x - x_0}{x_{\text{target}} - x_0}\right)$$
* **`SunItem`**: Manages falling sky suns and plant-produced suns with gravity physics, ground settling, click hitboxes, and smooth bezier collection flight toward the top-left SeedBank.
* **`LawnMower`**: Stationed at the left edge of each row. When a zombie breaches the lawn, the mower activates, rolling forward at high speed to clear all zombies in that lane.
* **`Vase`**: Breakable mystery containers used in Vasebreaker mode with leaf decals (plant) and zombie decals.

---

## Level Progression and Game Modes

### 1. Adventure Mode (Levels 1 to 6)

The adventure campaign provides progressive gameplay with distinct daytime and nighttime mechanics:

* **Daytime Stages (Levels 1–3)**:
  * Camera pan intro showing upcoming zombie threats.
  * "READY... SET... PLANT!" opening sequence.
  * 5x9 lawn grid plant placement with click-to-plant and shovel tools.
  * Sun economy with periodic sky drops.
  * Wave timers, progress bar tracking, huge wave announcements, and final wave flag rushes.
  * Victory award drop presentation and defeat sequences.
* **Nighttime Stages (Levels 4–6)**:
  * Dark atmosphere with zero sky sun drops, requiring sun-producing plants.
  * Procedurally placed gravestones (`GraveStone`) occupying grid cells.
  * Graves rise dynamically and spawn nocturnal zombies during final wave rushes.
  * `Gravebuster` integration for clearing cemetery obstacles.

### 2. Wall-nut Bowling Minigame (`BowlingLevel`)

A specialized minigame mode modeled on PopCap's conveyor belt mechanics:

* **Conveyor Belt UI**: Displays available Wall-nut cards moving smoothly from right to left with capacity gating (10 cards maximum).
* **Physics-Based Rolling Nuts**: Normal Wall-nuts bounce diagonally off top and bottom lawn boundaries upon impact with zombies. Giant Wall-nuts roll continuously through entire rows, crushing all zombies in their path. Explode-o-nuts detonate on first impact, clearing a 3x3 tile radius.

### 3. Vasebreaker Puzzle Mode (`VasebreakerLevel`)

A tactical puzzle mode where all plants and zombies are concealed inside mystery vases:

* **5x9 Vase Matrix**: Vases are divided between green plant vases and brown mystery/zombie vases.
* **Mallet Cursor Interaction**: Animated hammer strike with delayed hit debouncing (`VaseState::PendingBreak`) to prevent duplicate clicks during swing animations.
* **Dropped Seed Packets**: Shattered plant vases drop seed packets that fall to the ground with gravity physics, ready for tactical placement.

### 4. Zen Garden and Animation Inspector (`Testing`)

A real-time diagnostic visualizer allowing developers to spawn any plant or zombie, inspect keyframe tracks, test attack frame debouncing, and verify hitbox alignment.

---

## Design and Implementation of 5 OOP Design Patterns

To satisfy project requirements and maintain clean, decoupled software architecture, five design patterns are implemented across the engine:

```
[1. Singleton Pattern] -----> Resources, AudioManager, ProfileManager
[2. Factory Pattern]   -----> BowlingNut::Create, PlantFactory, ZombieFactory
[3. State Pattern]     -----> LevelPhase, QuizState, JalapenoState, IGameState
[4. Observer Pattern]  -----> EventManager, IObserver (Decoupled HUD/Audio)
[5. Strategy Pattern]  -----> ITrajectoryStrategy (Interchangeable Flight Math)
```

---

### 1. Singleton Pattern (Creational)

* **Current Implementation**: Implemented in `Resources::GetInstance()` (`include/core/resources.h`), `AudioManager::GetInstance()` (`include/core/AudioManager.h`), and `ProfileManager::GetInstance()` (`include/core/ProfileManager.h`).
* **Design Structure**: Implemented using the Meyer's Singleton idiom, utilizing static local instance initialization within `GetInstance()`. Constructors and destructors are private, and copy constructors and assignment operators are explicitly deleted.
* **Technical Rationale**: Centralizes GPU texture buffers, audio streams, and profile save states into single global access points, preventing redundant resource loading into GPU memory and avoiding race conditions during save file I/O.

```cpp
class Resources {
public:
    static Resources& GetInstance() {
        static Resources instance;
        return instance;
    }

    Resources(const Resources&) = delete;
    Resources& operator=(const Resources&) = delete;

    Texture2D GetTexture(const std::string& name) const;
    ReanimDefinition LoadReanim(const std::string& filePath);

private:
    Resources() = default;
    ~Resources() = default;
};
```

---

### 2. Factory Method Pattern (Creational)

* **Current Implementation**: Implemented in `BowlingNut::Create(plantType, x, y)` (`src/entities/plants/BowlingNut.cpp`), instantiating `NormalBowlingNut`, `GiantBowlingNut`, or `ExplodeBowlingNut` based on seed card identifiers.
* **Architecture Extension**: Generalized `PlantFactory` and `ZombieFactory` static creation interfaces replacing manual conditional construction blocks.
* **Technical Rationale**: Encapsulates entity instantiation, health assignment, animation path binding, and sun cost lookup, honoring the Open-Closed Principle (OCP) when adding new plant or zombie subclasses.

```cpp
enum class PlantType { PeaShooter, SnowPea, SunFlower, Wallnut, CherryBomb, Chomper, Jalapeno, Cornpult, Melonpult, FirePea };

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

* **Current Implementation**: Implemented across level phases (`LevelPhase::SeedSelection`, `PanToLawn`, `ReadySetPlant`, `ActiveWave` in `Level1.h`), quiz phases (`QuizState::Rules`, `Playing`, `AnswerFeedback`, `Summary` in `QuizMenu.h`), and entity life cycles (`JalapenoState::EXPLODING_SWELL`, `EXPLODING_FIRE`, `DONE` in `Jalapeno.h`).
* **Architecture Extension**: Polymorphic `IGameState` interface and `GameStateManager` context for managing high-level application screen transitions.
* **Technical Rationale**: Eliminates deeply nested switch-case update blocks in `main.cpp`, giving each game phase encapsulated `Enter()`, `Update()`, `Draw()`, and `Exit()` routines.

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

* **Design Structure**: Centralized `EventManager` acting as Subject/Dispatcher with an `IObserver` callback interface.
* **Technical Rationale**: Decouples gameplay triggers from dependent secondary subsystems:
  * Collecting a sun notifies `HUDManager` to increment the currency balance and `AudioManager` to play the collection sound without `SunItem` holding references to either class.
  * A zombie dying notifies wave progress trackers and score tallies.
  * A plant dying notifies `GridManager` to free the grid cell for replanting.

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
        if (m_listeners.find(event) != m_listeners.end()) {
            for (auto* obs : m_listeners[event]) obs->OnNotify(event, data);
        }
    }
};
```

---

### 5. Strategy Pattern (Behavioral)

* **Design Structure**: `ITrajectoryStrategy` interface defining position calculation algorithms over time, utilized by `Projectile`.
* **Technical Rationale**: Replaces boolean flag branching (`m_isLobbed`) in `Projectile.h` with interchangeable strategy algorithms for linear travel (`StraightTrajectoryStrategy`), parabolic catapult flight (`LobbedTrajectoryStrategy`), and radial splash damage (`AoeExplosionStrategy`).

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

#### macOS/Linux:

```bash
# Run the automated build script (applies mandatory CMAKE_POLICY_VERSION_MINIMUM shim)
cmake -S . -B build "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
cmake --build build --config Release --parallel 4

# Run executable
./build/PvZGame
```

#### Windows:

```powershell
# Run the automated build script (applies mandatory CMAKE_POLICY_VERSION_MINIMUM shim)
cmake -S . -B build "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
cmake --build build --config Release --parallel 4

# Run executable
.\build\PvZGame.exe
```
