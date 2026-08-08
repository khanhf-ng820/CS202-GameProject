# Q&A & Technical Troubleshooting Log

## 1. Unable to Delete `build/` Directory on Windows
* **Root Cause**: Windows locks files/folders when a process is using them (e.g. `cmake.exe`, `PvZGame.exe`, or a terminal whose working directory is inside `build/`).
* **Solutions**:
  * Kill locking background processes:
    ```powershell
    Stop-Process -Name "cmake" -Force
    ```
  * Ensure terminal working directory is at the project root (`cd ..`).
  * Remove using PowerShell force flags:
    ```powershell
    Remove-Item -Recurse -Force build
    ```
* **Note**: Manual folder deletion is rarely needed. Re-running `bash build.sh` automatically re-configures CMake with required flags (`-DCMAKE_POLICY_VERSION_MINIMUM=3.5`).

---

## 2. Git Pull Inspection Summary
* **Updated Files**: 15 files updated (Fast-forward merge).
  * **New Files**: `SeedSelectMenu.h`, `SeedSelectMenu.cpp`.
  * **Modified Files**: `AudioManager.h/cpp`, `Level1.h/cpp`, `Melonpult.h`, `MainMenu.cpp`, `OptionsMenu.h/cpp`, `SeedBank.h/cpp`, `README.md`, skills documentation.
* **Important Note**: Since `CMakeLists.txt` uses `file(GLOB_RECURSE)`, adding new `.cpp` files requires re-running the CMake configuration step:
  ```powershell
  cmake -S . -B build "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
  cmake --build build --config Release --parallel 4
  ```

---

## 3. Game Launch Path Fix
* **Root Cause**: Game failed to launch because the PowerShell terminal path was set inside `build\Debug`, resulting in invalid relative resolution (`build\Debug\build\Release\PvZGame.exe`).
* **Fix**: Change directory back to project root before running:
  ```powershell
  cd e:\CS202-GameProject
  .\build\Release\PvZGame.exe
  ```

---

## 4. Restored `.vscode/c_cpp_properties.json`
Restored VS Code C++ IntelliSense configuration to project default (GCC x64 MinGW):
```json
{
  "configurations": [
    {
      "name": "windows-gcc-x64",
      "includePath": [
        "${workspaceFolder}/**"
      ],
      "defines": [
        "_DEBUG",
        "UNICODE",
        "_UNICODE"
      ],
      "windowsSdkVersion": "10.0.26100.0",
      "compilerPath": "E:/mingw64/bin/gcc.exe",
      "cStandard": "${default}",
      "cppStandard": "${default}",
      "intelliSenseMode": "windows-gcc-x64",
      "configurationProvider": "ms-vscode.cmake-tools"
    }
  ],
  "version": 4
}
```

---

## 5. Animation FPS & Reanim Keyframe Interpolation
* **Game Frame Rate**: 60 FPS (set via Raylib `SetTargetFPS(60)` in `main.cpp`).
* **Peashooter Reanim Specs**: `<fps>12</fps>` in `PeaShooter.reanim` with 104 keyframe slots per track.
* **Why the Original Game looks Smooth**: PopCap's Reanim uses Flash skeletal/cutout keyframes (12 keyframes/sec sample rate). The engine performs real-time **linear interpolation (Lerp)** on transform attributes (position $X/Y$, scale $SX/SY$, rotation $KX/KY$) at 60 Hz.
* **Sub-frame Interpolation Implementation**:
  * Implemented `GetInterpolatedKeyframe` in `Reanimation.h` / `Reanimation.cpp`.
  * Calculates sub-frame fraction $\alpha = \text{frameFloat} - \lfloor \text{frameFloat} \rfloor$.
  * Smoothly blends translation, rotation, and scale across adjacent keyframes for all Plants, Zombies, Projectiles, and UI elements at 60 FPS, while preserving discrete frame event triggers (e.g. Peashooter projectile firing at frame 65).
