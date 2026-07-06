# CruUI — C++ UI library

Cross-platform C++23 UI library with platform-specific backends (Direct2D, Quartz, Cairo, Web Canvas, SDL). Header-only public API in `include/`, implementation in `src/`.

## Project

- **Language**: C++23 (strict, `CMAKE_CXX_EXTENSIONS OFF`)
- **Build**: CMake ≥3.21 with Ninja Multi-Config generator
- **Test framework**: Catch2 (bundled in `lib/Catch2`)
- **Dependencies**: ICU (required), plus platform-dependent: Cairo/Pango, SDL3, Direct2D, Quartz/CoreFoundation, XCB, vcpkg on Windows, Homebrew on macOS
- **Entry points**: library targets (CruBase → CruPlatform → CruUi), demos in `demos/`, ThemeBuilder tool in `src/ThemeBuilder/`

## Commands

```bash
# Configure (debug+release, static libs)
cmake --preset default

# Build only
cmake --build --preset debug
cmake --build --preset release

# Run all tests
ctest --preset debug
ctest --preset release

# Configure → build → test in one shot
cmake --workflow --preset build-and-test-debug

# CI workflow (what CI runs)
cmake --workflow --preset ci-default       # static linking
cmake --workflow --preset ci-dylib          # dynamic linking

# AddressSanitizer
cmake --preset asan
cmake --build --preset debug   # (inherits asan configure)

# On Windows: source the VC env script first
. scripts/Use-VC.ps1; Use-VC
```

## Architecture

| Module | Path | Role |
|--------|------|------|
| **CruBase** | `src/base/`, `include/cru/base/` | Foundation: Object/Interface base classes, events, strings, IO, XML/TOML parsers, logging, timers, subprocess, platform abstractions |
| **CruPlatform** | `src/platform/`, `include/cru/platform/` | Platform abstraction: graphics (Geometry, Brush, Painter, TextLayout), GUI (Window, Input, Clipboard, Cursor), color types, matrix math |
| **CruPlatformGraphics** | `src/platform/graphics/` | Graphics backends: **direct2d/** (Windows), **quartz/** (macOS), **cairo/** (Linux/optional-Windows), **web_canvas/** (Emscripten) |
| **CruPlatformGui** | `src/platform/gui/` | GUI backends: **win/** (Windows), **osx/** (macOS), **xcb/** + **sdl/** (Linux), plus bootstrap wiring |
| **CruUi** | `src/ui/`, `include/cru/ui/` | UI framework: controls (Button, TextBox…), render objects, layout (flex/stack), styling engine, data model types, routed events, components (Menu, PopupButton) |
| **CruParse** | `src/parse/`, `include/cru/parse/` | Parser combinator library (grammar, terminals/nonterminals, recursive descent, not developed yet) |
| **ThemeBuilder** | `src/ThemeBuilder/` | A real GUI tool built on CruUi that lets users to create theme |

Dependency chain: `CruBase → CruPlatformBase → CruPlatformGraphics + CruPlatformGui → CruUi`. Each module has its own `CRU_<MODULE>_API` DLL export macro.

## Conventions

- **Formatting**: Google style via `.clang-format`, `IncludeBlocks: Preserve`. 2-space indent for C++/ObjC, tabs for CMake. LF endings. Run `clang-format` before commit.
- **Headers**: `#pragma once`. Public headers in `include/cru/<module>/`, private in `src/<module>/`. Include path matches namespace: `cru::ui::controls::Control` → `cru/ui/controls/Control.h`.
- **Naming**: PascalCase for classes/structs/enums, `snake_case` for variables, `kPascalCase` for constants. Macros: `CRU_UPPER_SNAKE`. Test files: `*Test.cpp`.
- **Classes**: Inherit from `cru::Object` (non-copyable, non-movable) or `cru::Interface` (virtual base). Use `CRU_DELETE_COPY`, `CRU_DEFAULT_MOVE`, `CRU_DEFAULT_DESTRUCTOR` macros.
- **Platform code**: `#if WIN32`/`APPLE`/`UNIX`/`EMSCRIPTEN` guards in CMake and C++. Platform-specific sources under `src/<module>/platform/<os>/`.
- **Tests**: Catch2 `TEST_CASE` with descriptive names + tags. Tests register via `catch_discover_tests` (except Emscripten). Executable per module (e.g. `CruBaseTest`, `CruPlatformTest`).
- **Logging**: `CruLogInfo`/`CruLogWarn`/`CruLogError` macros with a `kLogTag` string constant.
- **Memory**: Prefer stack allocation and `std::unique_ptr`. Use `DeleteLater` pattern for controls during event handling (avoid deleting while iterating).

## Notes
