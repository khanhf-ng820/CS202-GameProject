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

1. Managed via an internal sub-state machine inside `Level1` using `enum class LevelPhase { SeedSelection, ActiveWave }`
   (see `game-state-and-levels` and `temp/preselect-design.md`).
   - Main Menu **"Level 1"** launches `Level1` starting in `LevelPhase::SeedSelection`.
   - Main Menu **"Start Adventure"** is reserved for the debug/reanimation testing visualizer (`Testing`).
2. Present available plant types in `SeedSelectMenu` as toggle-select cards.
   - Clicking an available chooser card adds it to `m_chosenPlants` (up to 7 max capacity) and places it into the top SeedBank bar, dimming the chooser grid card.
   - Clicking a selected card in either the top bar or chooser grid unselects it and restores the card.
3. Enforce a deck capacity (1 to 7 plants max) before allowing the player to confirm with **"LET'S ROCK!"**.
4. Store the confirmed selection as `m_chosenDeck` for the duration of that level and initialize `SeedBank` via `m_seedBank.initFromDeck(m_chosenDeck)` upon transitioning to `LevelPhase::ActiveWave`.

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
