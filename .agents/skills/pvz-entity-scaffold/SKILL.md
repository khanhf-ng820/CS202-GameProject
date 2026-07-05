---
name: pvz-entity-scaffold
description: Use whenever adding a brand-new Plant subclass or Zombie subclass to this project, or when reviewing/refactoring an existing one for consistency. Encodes the exact constructor, update(), and draw() conventions already followed by PeaShooter, SnowPea, Repeater, GatlingPea, Cornpult, Melonpult, Jalapeno, SunFlower, Wallnut, CherryBomb, Chomper, FirePea, ZombieNormal, and FlagZombie, so new entities don't drift from the established pattern. Always check this before writing a new .h/.cpp pair under include/ and src/ for a plant or zombie.
---

# PvZ Entity Scaffold

Every `Plant` subclass and `Zombie` subclass in this codebase follows the
same shape. Match it exactly for new entities — small inconsistencies
(e.g. `bool did_shoot = 0;` vs `bool did_shoot = false;`, forgetting the
debounce reset frame) are the actual bugs that show up when adding the 10th
plant type, not exotic logic errors.

## Header (`include/<Name>.h`)

```cpp
#pragma once
#include "Plant.h"   // or "Zombie.h"

class <Name> : public Plant {   // or : public Zombie
private:
    bool did_shoot = false;     // only if this entity fires a projectile
    // any other per-entity state (timers, flags) goes here

public:
    <Name>(Resources& res, int x, int y);          // Plant: int x, int y
    // <Name>(Resources& res, float x, float y);   // Zombie: float x, float y
    ~<Name>() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles,
                std::vector<SunItem>& outSuns) override;   // Plant signature
    // void update(float deltaTime) override;               // Zombie signature
    void draw() override;
};
```

Note the coordinate type difference: `Plant::m_x`/`m_y` are `int`,
`Zombie::m_x`/`m_y` are `float` (see `include/Plant.h` vs `include/Zombie.h`).
Mixing these up in a new subclass causes silent truncation, not a compile
error, when passed through constructors that take the other type.

## Constructor (`src/<Name>.cpp`)

```cpp
<Name>::<Name>(Resources& res, int x, int y)
    : Plant(res, x, y, /*hp*/300, /*sunCost*/100, "<Name>") {
    getResources(res.GetAssetPath("assets/reanim/<Name>.reanim"));
    m_anim.SetBaseAnimation("anim_idle");   // fallback shown when overlay track is hidden
    m_anim.SetAnimation("anim_shooting");   // or whichever anim should play by default
    // Optional: hide tracks this variant doesn't use, e.g.
    // m_anim.SetTrackVisible("anim_bucket", false);
}
```

- Always route the asset path through `res.GetAssetPath(...)` — never a
  hardcoded or relative path directly (see the `cmake-raylib-build` skill
  for why: the executable's working directory varies by generator/config).
- It's valid to reuse another entity's `.reanim` file when there's no unique
  asset yet — `Repeater` and `FirePea` both load `assets/reanim/PeaShooter.reanim`.
  Say so in a comment if you do this, so it's obviously intentional later.
- hp/sunCost/speed/damage are passed straight to the base constructor; don't
  duplicate them as separate member variables unless the subclass needs to
  change them at runtime (see `Wallnut`'s cracked-state handling for a case
  where the base `m_hp` is read but not shadowed).

## update()

```cpp
void <Name>::update(float deltaTime, std::vector<Projectile>& outProjectiles,
                     std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);

    if (m_anim.GetCurrentAnimName() == "anim_shooting"
        && m_anim.GetCurrentFrame() == 59 && !did_shoot) {
        Texture2D tex = res.GetTexture("ProjectilePea");
        outProjectiles.push_back(Projectile(m_x + 60, m_y + 35, 400.0f, tex));
        did_shoot = true;
    }
    if (m_anim.GetCurrentFrame() == 60) {
        did_shoot = false;   // MUST reset one frame after the trigger frame,
                              // or the debounce never re-arms and the entity
                              // only ever fires once, ever.
    }
}
```

- The debounce pattern (`did_shoot` set on frame N, cleared on frame N+1) is
  used identically in `PeaShooter`, `SnowPea`, `Repeater`, `GatlingPea`,
  `Cornpult` (frame 36/37), `Melonpult` (frame 49/50), and `FirePea`. Copy
  this shape exactly rather than reinventing a cooldown timer unless the
  entity genuinely needs elapsed-time-based firing (`SnowPea` additionally
  layers `m_fireTimer`/`m_fireRate` on top of the same debounce — see
  `src/SnowPea.cpp` for how the two combine).
- For Zombies, branch on death-animation names to hide parts and optionally
  spawn falling debris — see `ZombieNormal::update`'s
  `m_hasSpawnedDeathParts` block for the reference pattern (hide head/arm,
  push `FallingPart`s with randomized velocity, reset the spawned-flag once
  the animation leaves the death state).
- Multi-track hide/show branches (e.g. `FlagZombie::update`'s
  `anim_head1`/`anim_cone` visibility toggle on death anims) belong in
  `update()`, not `draw()` — visibility is animation state, not a
  per-frame render decision.

## draw()

```cpp
void <Name>::draw() {
    m_anim.Draw(m_x, m_y, 1.6f);   // 1.6f scale is the project-wide standard
}
```

- Every plant/zombie in this codebase draws at scale `1.6f` except
  `Chomper`, which uses `1.4f` — match `1.6f` unless you have a specific,
  documented reason not to (e.g. a visually oversized asset).
- A tint or overlay effect is fine (`FirePea` tints orange; `SunFlower`
  interpolates a golden glow before producing sun; `CherryBomb` draws a
  separate explosion sprite instead of the base animation while exploding)
  but should still call through `m_anim.Draw(...)` for the base case.

## Registering the new entity

There is currently no Factory (see `design-patterns-tracker`) — until that
refactor lands, follow `main.cpp`'s existing `currentPlantType`/
`currentZombieType` integer-switch convention exactly when wiring up a new
button, so the eventual Factory refactor has a consistent set of cases to
migrate. Do not invent a second, different registration mechanism alongside
the existing one.

`CMakeLists.txt` globs `src/*.cpp` automatically (`file(GLOB_RECURSE
SRC_FILES src/*.cpp)`), so a new `.cpp` file is picked up on the next CMake
*configure* — if the build was already configured, re-run
`cmake -S . -B build ...` (or just re-run `build.sh`), not only
`cmake --build`, or the new file won't be compiled.
