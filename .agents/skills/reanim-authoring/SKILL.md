---
name: reanim-authoring
description: Use whenever creating, editing, hand-writing, or debugging a .reanim animation file (the PopCap-style keyframe XML format used for all Plants vs. Zombies sprites), or when a plant/zombie animation looks wrong, flickers, shows the wrong pose, or won't loop correctly. Also use when adding a brand-new .reanim asset for a new Plant or Zombie subclass. Covers the -1 hidden-frame sentinel, keyframe value inheritance rules, and aligning a visual track's frame indices with the control tracks (anim_idle, anim_shooting, etc.).
---

# Reanim Authoring

This project's animation files (`peashooter.txt`, and every `assets/reanim/*.reanim`
file referenced by `res.GetAssetPath("assets/reanim/...")` in the Plant/Zombie
constructors) use a line-based keyframe XML format parsed by
`Resources::LoadReanim` in `src/resources.cpp` and rendered by
`Reanimation::Draw` in `src/Reanimation.cpp`. The parser is intentionally
simple (plain substring `ExtractTag` search per line), which means it has no
tolerance for format assumptions that aren't actually true. Read this before
hand-editing or generating any `.reanim`-style file.

## The four rules that actually matter

1. **Every `<t>` is one *global* frame index, shared across every track in
   the file.** `Reanimation::Draw` looks up `track.keyframes[currentFrame]`
   using the *same* `currentFrame` for every track. A visual track (e.g.
   `backleaf`, `Zombie_flaghand`) and a control track (e.g. `anim_idle`,
   `anim_walk`) at index 12 both refer to frame 12 of the whole animation —
   they are not independently indexed.

2. **`<f>-1</f>` means "not visible/active this frame." Any other value
   (usually `<f>0</f>`) means "visible/active."** This is checked as
   `kf.f != -1` everywhere in `Reanimation.cpp`. For *control* tracks
   (tracks whose name starts with `anim_` and whose keyframes never carry an
   `<i>` image tag), a contiguous run of non-`-1` frames defines the
   boundaries of that named animation — see `Reanimation::PopulateAnimations`.
   For *visual* tracks, `-1` just means "don't draw this track's image this
   frame," independent of which named animation is currently playing.

3. **Any tag omitted on a `<t>` line inherits the previous keyframe's value
   for that field** — not a default of 0. This is the inheritance loop at
   the top of the keyframe-parsing block in `Resources::LoadReanim`. A line
   like `<t><y>53.3</y></t>` only changes `y`; `x`, `sx`, `sy`, `kx`, `ky`,
   `f`, and the image name all carry over unchanged from the frame before it.
   A bare `<t></t>` therefore means "identical to the previous frame in
   every field," **not** "reset to nothing."

4. **The first `<i>IMAGE_...</i>` tag on a track sets the texture used until
   a later keyframe overrides it with a new `<i>` tag.** Frames after that
   don't need to repeat the `<i>` tag — `Reanimation::Draw` searches
   backwards through the track for the nearest prior non-empty image name
   when the current frame doesn't specify one.

## The exact pitfall to check for: leading-frame encoding

Whether a track's *first* frames are encoded as a leading `<f>-1</f>`
sentinel (hidden) versus a full keyframe block already present at frame 0
(visible immediately) changes what's on screen during those early frames —
and it is easy to get inconsistent between files that are supposed to
represent the same animation.

Concretely: in this repo, `peashooter.txt`'s `backleaf` track starts with a
bare `<f>-1</f>` at frame 0 and only gets its full `<x>/<y>/<sx>/<sy>/<f>0</f>/<i>`
block at frame 4. `peashooter_perfect.txt`'s `backleaf` track instead has
that *same* full block already at frame 0, then repeats it again at frame 4.
That is a real divergence between the two files, not a formatting quirk —
one of them shows the backleaf during frames 0–3 and the other doesn't.

**Before saving any edited or newly authored `.reanim`-style file:**

- Identify which control track(s) should make this visual track visible, and
  the frame range where that control track is active (non `-1`).
- Deliberately choose whether the visual track's leading frames should be
  `<f>-1</f>` (hidden until its animation starts) or a full keyframe at
  frame 0 (visible immediately) — don't let this drift between files that
  are meant to represent the same character/animation.
- Confirm every track in the file has the same total number of `<t>` entries.
  A short track silently stops rendering once `currentFrame >= keyframes.size()`
  is exceeded, which shows up as a part of the sprite disappearing partway
  through playback with no error anywhere.
- If you're unsure which encoding is correct, prefer matching whichever
  sibling file in the same asset family already renders correctly in-game,
  and note the discrepancy to the user rather than silently picking one.

## When adding a brand-new .reanim file for a new Plant/Zombie

1. Confirm the `<fps>` tag at the top matches the other files (12 in this
   project) unless you have a specific reason to differ.
2. Build control tracks (`anim_idle`, `anim_walk`, `anim_shooting`, etc.)
   first, and get their active (non `-1`) ranges right, before filling in
   the visual tracks that depend on those ranges.
3. Reuse an existing file's structure as a template rather than writing one
   from scratch — this project already reuses `PeaShooter.reanim` visually
   for `Repeater` and `FirePea` (see `src/Repeater.cpp`, `src/FirePea.cpp`),
   which is a valid pattern when you don't have a unique asset yet.
4. After authoring, load it through the actual game (or a minimal test that
   calls `Resources::LoadReanim` and inspects `def.tracks`/`def.maxFrames`)
   rather than assuming correctness from reading the text.
