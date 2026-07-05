---
name: cmake-raylib-build
description: Use whenever modifying CMakeLists.txt, build.sh, the GitHub Actions workflow, adding a new dependency, or debugging a build/configure failure in this project. Covers the CMAKE_POLICY_VERSION_MINIMUM shim, the FetchContent pins for raylib/raygui/nlohmann_json, why raygui is wrapped as an INTERFACE target, and the cross-platform CI matrix. Always check this before touching build configuration or troubleshooting a "why won't this compile/configure" problem.
---

# CMake + Raylib Build

## The CMAKE_POLICY_VERSION_MINIMUM shim — do not remove

`CMakeLists.txt` sets `CMAKE_POLICY_VERSION_MINIMUM 3.5`, and `build.sh` and
the GitHub Actions workflow both also pass
`-DCMAKE_POLICY_VERSION_MINIMUM=3.5` on the command line. This exists
because CMake 4.0+ dropped compatibility with very old
`cmake_minimum_required` declarations, and raylib/raygui's own bundled
`CMakeLists.txt` files predate that modern minimum. Without this shim,
`FetchContent_MakeAvailable(raylib raygui json)` fails to configure on any
machine with a recent CMake. If a configure step ever fails with a policy
version error, check that this flag is still present in all three places
before assuming the dependency versions need to change.

## Dependencies (`FetchContent`)

| Dependency | Source | Pinned to | Notes |
|---|---|---|---|
| raylib | `github.com/raysan5/raylib.git` | tag `5.0` | `BUILD_EXAMPLES`/`BUILD_GAMES` forced OFF |
| raygui | `github.com/raysan5/raygui.git` | tag `4.0` | **Header-only.** Not a compiled target — see below. |
| nlohmann_json | release `.tar.xz` URL (v3.11.3) | v3.11.3 | Fetched via a direct archive URL, not git |

`raygui` has no compiled library target, so it's wrapped manually:

```cmake
add_library(raygui_interface INTERFACE)
target_include_directories(raygui_interface INTERFACE ${raygui_SOURCE_DIR}/src)
```

Link against `raygui_interface`, not `raygui` — there is no `raygui` target
to link against, and trying to will fail with "target not found."

`nlohmann_json` exports a namespaced target: link
`nlohmann_json::nlohmann_json`, already present in the main
`target_link_libraries(...)` call. This dependency is currently unused in
the codebase — it was fetched specifically for the save/load feature (see
`game-state-and-levels`), so don't remove it just because nothing calls it
yet.

## Source discovery

```cmake
file(GLOB_RECURSE SRC_FILES src/*.cpp)
```

New `.cpp` files under `src/` are picked up automatically — but only at
**configure** time, not build time. If a project was already configured
before a new file existed, running `cmake --build build` alone will not
pick it up; re-run the full configure step (`cmake -S . -B build
-DCMAKE_POLICY_VERSION_MINIMUM=3.5`, i.e. re-run `build.sh`) first.

## Asset path resolution

`Resources::GetAssetPath` (in `src/resources.cpp`) checks the given
relative path, then `../` + path, then `../../` + path, then `../../../` +
path, falling back to the original if none exist. This exists because the
built executable's working directory differs by generator/config (e.g.
`build/`, `build/Debug/`, `build/Release/`). Every existing Plant/Zombie
constructor routes its asset path through this function
(`res.GetAssetPath("assets/reanim/...")`) — always do the same for new
assets rather than hardcoding a path or assuming a particular working
directory.

## Cross-platform CI (`.github/workflows/cmake-multi-platform.yml`)

Builds a matrix of Windows/MSVC, Linux/gcc, Linux/clang, and macOS/clang.
Practical implications:

- Avoid GNU-specific extensions or non-portable includes — MSVC will fail
  even if gcc/clang accept them.
- The Linux job explicitly installs X11/audio/GL dev packages via
  `apt-get install` for raylib's dependencies. If a new raylib feature (or
  a new dependency you add) needs an additional system library on Linux,
  update that `apt-get install` line in the workflow — CI will fail even
  though a local build succeeds if a machine already happens to have the
  library installed.
- `ctest` runs after build in CI — if you add tests, make sure they're
  registered with CMake's test framework so CI actually exercises them.

## Local build entrypoint

Prefer running `build.sh` (`cmake -S . -B build
-DCMAKE_POLICY_VERSION_MINIMUM=3.5` then `cmake --build build --config
Release --parallel 4`) over ad-hoc `make`/`ninja` invocations, so local
behavior matches what CI does.
