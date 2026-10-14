# ZAIDX FPS BOOSTER — Phase 9

Phase 9 adds a conservative Gaming Mode workflow.

## Added
- A dedicated Gaming Mode action.
- Applies `HIGH_PRIORITY_CLASS` only to the user-selected process.
- Action is temporary/reversible and does not create persistence.
- Existing read-only CPU/RAM/thread telemetry and foreground/game detection remain.

## Intentionally excluded
- DLL injection or hooks
- anti-cheat bypass
- process-memory modification
- Defender/security tampering
- registry persistence
- driver manipulation
- license/trial bypass

## Build
cmake -S . -B build
cmake --build build --config Release
