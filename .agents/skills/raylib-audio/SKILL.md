---
name: raylib-audio
description: Use when adding sound effects or background music to this project. There is currently zero audio code anywhere in the codebase — Resources::LoadAll only loads .png textures, InitAudioDevice is never called, and no .wav/.ogg files are referenced. This directly targets the rubric's "Sounds (10 points): effect, background" line item. Always check this before wiring up any shoot/plant/death/collect sound or menu music.
---

# Raylib Audio

## What's missing today

`Resources::LoadAll` (in `src/resources.cpp`) walks a directory and loads
only files whose extension is `.png` into a `textures` map. There is no
`InitAudioDevice()` call in `main.cpp`, no `Sound`/`Music` members on
`Resources`, and no audio asset files referenced anywhere. This is a clean
slate — the rubric explicitly asks for both effect sounds *and* background
music (10 points), so don't stop at just one.

## Setup (once, in `main.cpp`)

```cpp
InitAudioDevice();   // before the main loop, alongside InitWindow(...)
// ...
CloseAudioDevice();  // at shutdown, alongside CloseWindow()
```

## Extend `Resources` to load sounds

Mirror the existing texture-loading loop rather than inventing a different
mechanism:

```cpp
// in Resources::LoadAll, alongside the existing .png loop
if (lastDot != std::string::npos &&
    (path.substr(lastDot) == ".wav" || path.substr(lastDot) == ".ogg")) {
    std::string stem = GetFileStem(path);
    std::string key = ToUpper(stem);
    Sound snd = LoadSound(path.c_str());
    if (snd.frameCount > 0) {
        sounds[key] = snd;
    }
}
```

Add a `GetSound(name)` accessor analogous to `GetTexture(name)` — but do
**not** copy `GetTexture`'s `IMAGE_REANIM_`/`IMAGE_` prefix-stripping logic
verbatim; sound asset names won't use that prefix scheme, so a plain
`ToUpper` lookup is correct here. Remember to call `UnloadSound` for every
entry in `Resources::UnloadAll`, mirroring the existing texture cleanup loop.

## Background music needs a per-frame update call

```cpp
Music bgMusic = LoadMusicStream(res.GetAssetPath("assets/audio/bgm.ogg").c_str());
PlayMusicStream(bgMusic);
// ... inside the main loop, every frame, while music should be playing:
UpdateMusicStream(bgMusic);
```

Forgetting `UpdateMusicStream` every frame is the single most common raylib
audio bug — the music will appear to start and then silently stop after a
fraction of a second with no error. If pausing is implemented (see
`game-state-and-levels`), skip the `UpdateMusicStream` call while paused
rather than calling `PauseMusicStream`/`ResumeMusicStream` inconsistently —
pick one approach and use it everywhere.

## Where to hook effect sounds into existing code

Every one of these hook points already exists in the codebase — no new
timers or state needed, just a `PlaySound` call at an existing branch:

| Sound | Exact hook point |
|---|---|
| Pea shoot | The `did_shoot` debounce block in `PeaShooter::update`, `SnowPea::update`, `Repeater::update`, `GatlingPea::update`, `FirePea::update` (frame 59) |
| Corn/melon launch | The `did_shoot` block in `Cornpult::update` (frame 36), `Melonpult::update` (frame 49) |
| Cherry bomb explosion | `CherryBomb::update`, where `is_exploding_effect` first becomes true |
| Jalapeno fire wave | `Jalapeno::update`, the `EXPLODING_SWELL` → `EXPLODING_FIRE` transition |
| Sun produced | `SunFlower::update`, where `m_sunProduceTimer` resets after reaching `m_sunProduceInterval` |
| Sun collected | Wherever the click-to-collect logic lands (see `grid-collision-economy` — `SunItem` doesn't yet detect clicks) |
| Zombie death | `ZombieNormal::update`'s `m_hasSpawnedDeathParts` block; the equivalent death-animation branch in `FlagZombie::update` |
| Plant placed / seed selected | Wherever planting is confirmed (see `grid-collision-economy`) and wherever a loadout choice is confirmed (see `seed-deck-loadout`) |

## Rubric checklist

- [ ] At least one "effect" sound wired to a real gameplay trigger (not just
      loaded and never played).
- [ ] Background music that plays continuously during `GameState::Playing`
      (see `game-state-and-levels`), with `UpdateMusicStream` called every
      frame it should be audible.
- [ ] `InitAudioDevice`/`CloseAudioDevice` bracket the whole program, not
      just one level, so switching levels doesn't reinitialize audio.
