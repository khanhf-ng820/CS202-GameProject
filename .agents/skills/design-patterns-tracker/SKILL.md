---
name: design-patterns-tracker
description: Use whenever implementing a new gameplay feature, refactoring plant/zombie/game-manager code, adding a new subsystem, or preparing design documentation for this project. This course rubric requires at least 5 documented design patterns worth 25 of 100 points, and this skill tracks which patterns are already implemented, where, and why, so new code satisfies an unmet pattern instead of adding ad-hoc logic. Always consult this before writing a manager class, a creation/spawning mechanism, an event/notification path, or anything with distinct behavioral "modes."
---

# Design Patterns Tracker

The course rubric requires "at least 5 design patterns (Factory, Singleton,
Observer, ...)" and separately grades "Effective use of 5 design patterns"
for 25 of 100 points. Do not force a pattern where it genuinely doesn't fit —
but whenever a new feature could naturally satisfy one of the still-missing
patterns below, prefer that approach over ad-hoc logic, since 5 clean
patterns score much better than 8 shallow/incorrect ones.

## Current pattern status

Keep this table current. Edit it in place whenever you add, extend, or
refactor a pattern — this file is the source of truth for the project's
design documentation deliverable (see the `design-doc-generator` skill).

| Pattern | Status | Location | Notes |
|---|---|---|---|
| Singleton | **Done** | `Resources::GetInstance()` in `src/resources.cpp` | Classic Meyer's singleton (`static Resources instance;` in a function). Leave as-is. |
| Factory Method | **Informal only** | `main.cpp`, the `if (currentPlantType == 0) ... else if ...` chains for plant/zombie construction | Replace with `PlantFactory::Create(PlantType type, Resources& res, int x, int y) -> std::unique_ptr<Plant>` and a parallel `ZombieFactory`. This directly cleans up the ugliest code in `main.cpp` and turns an implicit pattern into a real, defensible one. |
| State | **Partial** | `JalapenoState` enum (`EXPLODING_SWELL` / `EXPLODING_FIRE` / `DONE`) in `include/Jalapeno.h` | Good small-scale example already exists. Formalize a project-wide `GameState` (see `game-state-and-levels` skill) using the same enum-driven-branch approach, or promote to real State objects with a common interface if you want a cleaner textbook example for the design doc. |
| Observer | **Not started** | — | Strong fit: "sun collected" → HUD update, "zombie died" → score/wave-tracker update, "plant destroyed" → grid cell freed for replanting. Decouples `main.cpp`'s draw/update loop from scoring and UI, which currently have no separation at all. |
| Strategy | **Not started** | — | `Projectile`'s `m_isLobbed` bool plus `m_range`/`m_maxHeight` (in `include/Projectile.h`) is already an interchangeable-trajectory problem hiding as a flag. Extract a `TrajectoryStrategy` interface with `StraightStrategy` and `LobbedStrategy` implementations; `Melonpult`/`Cornpult` (lobbed) vs. `PeaShooter`/`SnowPea`/`Repeater`/`GatlingPea` (straight) are the natural test cases. |
| Command (optional 6th) | **Not started** | — | Wrap "plant seed of type T at grid cell (r,c)" as a Command object with an `Execute()`. Makes the save/load feature in `game-state-and-levels` and an undo-last-placement feature almost free, and is an easy 6th pattern if a grader is strict about the 5. |

## Workflow for every new feature

1. **Check this table first.** Ask: "Am I spawning a new kind of object?"
   → Factory. "Does something else need to react when this happens, without
   this code knowing about it?" → Observer. "Does this object behave
   differently depending on a phase/mode?" → State or Strategy.
2. **Prefer extending an already-Done/Partial pattern** over introducing a
   brand-new one, unless the new one is clearly the better fit. Depth on
   fewer patterns beats breadth across many shallow ones for this rubric.
3. **After touching a pattern, update the table above** with the specific
   file/class you touched and a one-line reason. Do this in the same commit
   as the code change — don't let this file go stale.
4. **Don't invent a pattern that isn't real.** A single `if/else` is not a
   Strategy pattern; a class with one static getter is not automatically a
   Singleton unless it actually protects a single shared instance the way
   `Resources` does. Graders can tell the difference, and a wrong claim in
   the design doc is worse than one fewer pattern.

## Feeding the design documentation deliverable

When `design-doc-generator` (or you directly) produces class diagrams or
sequence diagrams, this table is the checklist of which patterns need a
diagram and a short written justification. A pattern that's "Not started"
in this table should not appear in the design doc yet — don't document
aspirational patterns as if they're implemented.
