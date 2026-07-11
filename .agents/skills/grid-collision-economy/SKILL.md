---
name: grid-collision-economy
description: Use when implementing the lawn grid, click-to-plant placement, the sun currency economy, or any collision detection between Projectiles, Zombies, and Plants. This is the single largest rubric category ("Player Inputs, Movement and Collision" — 20 points, plus "Power-Ups and Items" — 10 points) and currently none of it exists — Projectile, Zombie, and Plant never check against each other anywhere in the codebase. Always check this before adding planting, sun collection, or damage-on-contact logic.
---

# Grid, Collision, and Sun Economy

## What's missing today

There is currently no lawn grid, no click-to-plant flow, no sun-currency
gate on placement, and — critically — **no collision detection anywhere**.
`Projectile::update` only advances its own position; nothing checks whether
a `Projectile` has reached a `Zombie`. Nothing checks whether a `Zombie`
has reached a `Plant`. This is the biggest single point gap in the project
relative to the rubric — prioritize this before polishing anything else.

## Lawn grid math

```cpp
constexpr int LAWN_COLS = 9;
constexpr int LAWN_ROWS = 5;
constexpr float CELL_WIDTH = 80.0f;
constexpr float CELL_HEIGHT = 100.0f;
constexpr float LAWN_ORIGIN_X = 320.0f;  // clear of the UI panel, which is 320px wide in main.cpp
constexpr float LAWN_ORIGIN_Y = 80.0f;

// pixel -> grid
int col = (int)((mousePos.x - LAWN_ORIGIN_X) / CELL_WIDTH);
int row = (int)((mousePos.y - LAWN_ORIGIN_Y) / CELL_HEIGHT);

// grid -> pixel (Plant::m_x/m_y are int — this lines up directly)
int px = (int)(LAWN_ORIGIN_X + col * CELL_WIDTH);
int py = (int)(LAWN_ORIGIN_Y + row * CELL_HEIGHT);
```

Reject clicks where `col`/`row` fall outside `[0, LAWN_COLS)` /
`[0, LAWN_ROWS)`, and reject planting on a cell that already has a live
plant in it (track occupancy in a `std::array<std::array<Plant*, LAWN_COLS>,
LAWN_ROWS>` or similar — don't rely on scanning a flat vector every click).

## Row/lane as a first-class field

Neither `Plant` (`include/Plant.h`) nor `Zombie` (`include/Zombie.h`)
currently stores which lawn row it occupies. Add an `int m_row` (or
`m_lane`) member to both base classes and thread it through every
constructor. This is a breaking change to every existing subclass's
constructor call — coordinate with the `pvz-entity-scaffold` skill's
recipe when doing this, and update every `Plant`/`Zombie` subclass
constructor in the same change rather than leaving some entities row-less.
Once every entity knows its row, collision checks only need to compare
entities in the same row, not the full O(n×m) cross-product.

## Sun economy

- Track `int sunCount` on the `GameManager` singleton (see
  `game-state-and-levels`), not as a loose global in `main.cpp`.
- `SunFlower` already produces `SunItem`s on a timer
  (`m_sunProduceInterval`), and `SunItem::update`/`draw` already handle its
  float/fade animation — but nothing currently detects a click on a
  `SunItem` to collect it. Add a click check (point-in-radius against the
  `SunItem`'s current `x`/`y`) that increments `sunCount` and marks it
  inactive.
- Gate planting on `sunCount >= plant->getSunCost()` — `Plant::getSunCost()`
  already exists as a public getter, so this check is a one-liner once the
  click-to-plant flow exists. Decrement `sunCount` only on successful
  placement (occupied-cell and insufficient-sun clicks should both be
  silent no-ops, not partial charges).

## Collision

For each frame while `GameState::Playing`:

1. **Projectile vs. Zombie**: for each active `Projectile`, for each
   `!zombie->isDead()` `Zombie` in the same row, check AABB or simple
   distance overlap. On hit: call `zombie->takeDamage(damage)` (already a
   public virtual on `Zombie`) and deactivate the projectile (the
   `Projectile` class doesn't currently expose a way to force-deactivate
   from outside — add a small `Deactivate()` method rather than reaching
   into private state).
2. **Zombie vs. Plant**: for each `!zombie->isDead()` `Zombie` in the same
   row as an occupied grid cell, if within biting range, call
   `plant->takeDamage(m_damage)` (already public on `Plant`, using the
   zombie's own `getDamage()`) every frame (or on a small interval) it stays
   in range, and call `zombie->setEating(true)`/`isEating()` (already public
   on `Zombie`) to drive the eating animation instead of walking. Clear
   `setEating(false)` once the plant dies or the zombie moves past it.
3. **Zombie vs. house**: see `game-state-and-levels` for the lose condition
   — this is the same per-frame pass, just checking `getX()` against the
   house boundary instead of a plant.

Keep all three checks in one pass over "zombies in this row" rather than
three separate full scans — this project has no spatial partitioning beyond
row, so row-scoping is the only performance lever available and is enough
at PvZ's scale (a handful of zombies per row).
