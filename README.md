# ZAIDX FPS BOOSTER — Phase 21

Phase 21 focuses on source-audit cleanup and reliability.

## Changes
- Audited and repaired a malformed escaped newline in the `system_info.cpp` include block if present.
- Normalized the process-list PID terminator check to the standard wide null-character literal.
- Kept prior phases' guarded process-priority actions, selection checks, GPU telemetry retry behavior, and performance history protections.
- Added a repeatable source-audit checklist.

## Build (Windows)
Requirements: Windows 10/11, Visual Studio 2022 with Desktop development with C++, and CMake 3.20+.

```bat
cmake -S . -B build
cmake --build build --config Release
```

## Validation status
- ZIP integrity and targeted source assertions passed.
- This environment did not run a Windows compiler or real-game runtime tests.

## Limitations
GPU Engine counters are best-effort activity indicators and may be unavailable. GPU temperature is not included. High priority does not guarantee better FPS. TEMP cleanup is destructive and requires confirmation. This clean-room reconstruction is not the original source code and does not include injection, anti-cheat bypass, game-memory modification, persistence, or security disabling.
