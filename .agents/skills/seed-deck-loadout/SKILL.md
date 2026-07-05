---
name: seed-deck-loadout
description: Use when building the pre-level plant-selection ("seed deck") screen, enforcing which plants can be placed during a level based on the chosen deck, or persisting which plants are unlocked. This is this project's answer to the rubric's "Multiple Players" item (a character-selection screen adapted from Mario/Luigi switching to PvZ's actual mechanic of choosing your plant loadout). Always check this before building the plant-selection UI or gating plant placement by availability.
---

# Seed Deck Loadout

## Why this exists

The course rubric's "Multiple Players" item (5 points) was written for a
Mario clone: "add a character selection screen... allow the player to
switch between Mario, Luigi, or other characters, each with different
abilities." A PvZ clone has no equivalent player-controlled character to
swap, so this project's chosen interpretation is a **seed-deck loadout
screen**: before each level, the player picks which plant types they're
allowed to use, mirroring the spirit of a character-select screen with
PvZ's actual core mechanic. This is a deliberate interpretation, not the
literal rubric wording — the design documentation (see
`design-doc-generator`) should say so explicitly rather than implying this
was the rubric's original intent.

## Flow

1. Entered as `GameState::SeedSelect` (see `game-state-and-levels`),
   between `MainMenu`/level-complete and `Playing`.
2. Present every plant type in `unlockedPlants` (from the save file) as a
   toggle-select button — reuse and adapt the existing plant-preview button
   layout already in `main.cpp` (the `DrawButton` calls for "Peashooter",
   "Snow Pea", etc.) rather than building a new button system from scratch,
   but change the interaction model from "select one to preview" to
   "toggle N of many to build a deck."
3. Enforce a minimum (e.g. at least 1) and optionally a maximum deck size
   before allowing the player to proceed to `Playing`.
4. Store the confirmed selection as `currentDeck` for the duration of that
   level (see the JSON shape in `game-state-and-levels` — keep field names
   in sync between the two skills).

## Enforcement during play

Once `GameState::Playing` starts, the click-to-plant flow (see
`grid-collision-economy`) must check the clicked seed-packet's plant type
against `currentDeck` before allowing placement — reject (and visually
gray out) any seed-packet button for a plant not in the current deck, even
if it's in `unlockedPlants`. Being unlocked and being in the active deck are
two different conditions; don't collapse them into one check.

## Persistence: two different lists, don't conflate them

- `unlockedPlants` — grows permanently as levels are completed. This is
  "which characters exist for you to choose from," the actual rubric
  analogue.
- `currentDeck` — chosen fresh each time `SeedSelect` runs; resets or
  defaults to the previous choice, but is not itself permanent progress.

Both live in the same save file described in `game-state-and-levels` — read
that skill's JSON schema before adding fields here so the two skills don't
invent conflicting names for the same concept.

## Documenting this choice

Because this is an interpretive answer to an ambiguous rubric item rather
than a literal implementation of "switch between Mario and Luigi," make
sure `design-doc-generator`'s written description explicitly states the
interpretation and the reasoning (matching the rubric's *intent* — a
selection screen offering meaningfully different play styles — rather than
its *literal* Mario-specific wording). A grader who has to infer this
themselves is a grader more likely to mark it as not satisfying the
requirement.
