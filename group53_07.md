# Weekly Report

## General Information

- Group ID: Group 53
- Group Name: Group 53
- Project Name: Plants vs. Zombies
- Date Range: 2026-07-19 to 2026-07-25

## Work Completed This Week

### Member 1: Từ Hoàng Anh - 25125005

| Date | Commit | Message |
| --- | --- | --- |
| 2026-07-22 | a18aeab | add card packets |
| 2026-07-22 | 06f8c34 | update the animations, add level 1(testing phase) |
| 2026-07-22 | 86f5982 | Update cmake |
| 2026-07-23 | 2a689b8 | update theme for level 1 |
| 2026-07-24 | 02f4ac0 | fix the projections and appearance for the peashooter |
| 2026-07-25 | ce95bdb | add mini decoration for the projectile |
| 2026-07-25 | 77abf80 | move file to the destination |
| 2026-07-25 | b5e118d | fix cmake to handle file confirgure |

#### Completed Tasks

- Added card packet structure and logic for plant loadout choices in seed selection.
- Created initial Level 1 testing phase with lawn grid, wave updates, and updated background theme in [Level1.h] / [Level1.cpp].
- Refactored plant animations, particle effects, and projectile appearances for Peashooter in [PeaShooter.cpp], [Projectile.cpp], and [particle.cpp].

### Member 2: Nguyễn Phúc Khánh - 25125086

| Date | Commit | Message |
| --- | --- | --- |
| 2026-07-22 | d044656 | chore: edited README.md with more detail |
| 2026-07-23 | ad730ab | fix: set 800x600 as default window resolution |
| 2026-07-23 | 4369e92 | chore: swapped positions of label buttons in options menu |
| 2026-07-24 | a52ac91 | feat: added blank shop menu screen |
| 2026-07-24 | 4741e48 | feat: added asset images to shop menu |
| 2026-07-24 | c862fe0 | fix: added asset fonts for sun counter and seed cost text |
| 2026-07-24 | ea49192 | fix: changed positions and sizes of Crazy Dave, the seed packets, "Main Menu" button in shop menu |
| 2026-07-24 | e24e1f6 | feat: added PREV and NEXT buttons for navigation in shop menu |
| 2026-07-25 | 35fc4a5 | refactor: reorganized files in include/ and src/ into subfolders |
| 2026-07-25 | a947a3d | feat: added BGM for main menu and shop menu screens |
| 2026-07-25 | fd9d702 | add: main menu and hi-hat sound tracks |
| 2026-07-25 | deff75a | feat: added music volume slider in options menu |

#### Completed Tasks

- Reorganized project directory structures and updated CMake configuration in [CMakeLists.txt] with `CONFIGURE_DEPENDS` for source file auto-discovery.
- Reconfigured window startup settings in [main.cpp] and [OptionsMenu.cpp] to default to 800x600 ("Compact" 4:3 aspect ratio).
- Created Crazy Dave's Shop interface ([ShopMenu.h] / [ShopMenu.cpp]), rendering Crazy Dave, the car trunk background, 8 seed packet slots in a 2x4 grid, PREV/NEXT navigation controls, and main menu exit button.
- Integrated interactive Shop button on main menu sign in [MainMenu.cpp] with pixel-perfect hover highlighting.
- Extended custom font rendering in [BitmapFont.cpp] using ContinuumBold metrics for the sun counter display and Pico-129 metrics for seed packet prices.
- Implemented global audio system ([AudioManager.h] / [AudioManager.cpp]), adding background music streams for Main Menu and Shop screens, sound effects, and a music volume slider control in [OptionsMenu.cpp].
- Reorganized project header and source files into modular directory structures (`include/core`, `include/ui`, `include/level`, `include/entities`, etc.).

## Tasks Planned for Next Week

- Nguyễn Phúc Khánh: Implement shop purchasing logic, seed packet unlocking persistence, and integrate the pre-level Seed Deck loadout selection UI.
- Từ Hoàng Anh: Expand Level 1 zombie wave progression, refine plant placement grid collisions, and complete sun item generation/collection logic.

## Issues

- CMake did not automatically re-glob recursive source files when new files were added to subdirectories. Resolved by setting `file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)` in `CMakeLists.txt`.
- Duplicate source file `src/PeaShooter.cpp` caused symbol redefinition conflicts during linking. Removed redundant file in favor of `src/entities/plants/PeaShooter.cpp`.

## Validation and Integrity

- No code was directly copied without verification.
- All changes were compiled and verified locally.
- CMake build configuration was tested with policy shims to ensure cross-platform compilation stability.

## Prompts Used

### Conversation: Making Shop In Game (de4b6fd4-43fc-4aef-9ddd-6c6764b28327)

#### Prompt 1

- **Sent At**: 2026-07-23T13:10:49Z
- **Prompt Content**:
```text
/grill-me Analyze the entire codebase. Your task is to change the default resolution to the "Compact" one. Create an implementation plan first, and wait for my approval.
```
- **Accomplished Tasks**: Reconfigured application startup settings in `main.cpp` and `OptionsMenu.cpp` to launch at the 800x600 (4:3 aspect ratio) "Compact" resolution preset by default.

---

#### Prompt 2

- **Sent At**: 2026-07-23T15:06:44Z
- **Prompt Content**:
```text
/grill-me Analyze the entire codebase and @[assets] folder. Add a Shop button in the main menu screen that uses pixel-perfect mouse hovering highlighting. Clicking the Shop button should transition to the Shop menu screen. Create an implementation plan first, and wait for my approval.
```
- **Accomplished Tasks**: Created the interactive "Crazy Dave's Shop" button region on the main menu wood sign in `MainMenu.cpp` with pixel-perfect hover highlighting, linking it to the new `GameState::SHOP` menu state.

---

#### Prompt 3

- **Sent At**: 2026-07-24T04:17:39Z
- **Prompt Content**:
```text
/grill-me In the Shop menu, add crazy dave and his car, and the seed packets. Clicking the seed packets doesn't do anything yet. Create an implementation plan first, and wait for my approval.
```
- **Accomplished Tasks**: Designed and implemented `ShopMenu.h` and `ShopMenu.cpp`, loading and rendering Crazy Dave (`CrazyDave.png`), the car trunk background (`Store_Car.png`), and seed packet placeholders.

---

#### Prompt 5

- **Sent At**: 2026-07-24T14:16:05Z
- **Prompt Content**:
```text
/grill-me Analyze the updated codebase. In the Shop menu, for the 8 seed packets, reposition them like so:
4 cards on first row:
- First card: `(x,y)` coordinates are: top left `(186*2, 155*2)`, bottom right `(211*2, 190*2)`.
- Second card: top left `(223*2, 155*2)`, bottom right `(248*2, 190*2)`.
- 3rd and 4th cards follow the same row and same spacing.
4 cards on second row:
- First card: `(x,y)` coordinates are: top left `(210*2, 103*2)`, bottom right `(235*2, 138*2)`.
- Second card: top left `(247*2, 103*2)`, bottom right `(272*2, 139*2)`.
- 3rd and 4th cards follow the same row and same spacing.
Create an implementation plan first, and wait for my approval.
```
- **Accomplished Tasks**: Updated grid coordinate vectors in `ShopMenu.cpp` to accurately position 8 seed packets across 2 rows of 4 cards on the shelves of Crazy Dave's trunk.

---

### Conversation: Daytime Level Setup Plan (947fe065-a531-400d-8838-1ef148039015)

#### Prompt 4

- **Sent At**: 2026-07-24T05:53:22Z
- **Prompt Content**:
```text
/grill-me The Seed Packet Price Font is not `Pix118Bold`, but `Pico-129` instead. Also, use `assets/data/_ContinuumBold.png` and `assets/data/_ContinuumBold.txt` for the Sun Counter Font. Create an implementation plan first, and wait for my approval.
```
- **Accomplished Tasks**: Extended `BitmapFont.cpp` to load character glyph metrics from `_ContinuumBold.txt` and `Pico-129.txt`, successfully rendering custom asset fonts for the sun counter display and plant seed costs.
