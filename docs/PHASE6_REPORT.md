# ZAIDX FPS BOOSTER — Phase 6

Phase 6 adds a lightweight game-detection workflow without injecting code into games.

## Added
- Detect the current foreground application/process.
- Display its executable name and PID.
- Heuristic "game-like" classification using common executable-name hints.
- Existing process selector, priority controls, CPU/RAM/GPU monitoring, power-plan option, TEMP cleanup and read-only affinity inspection remain.

## Important limitation
This is not an FPS counter. Exact FPS measurement would normally require integration with a game's rendering path, a supported overlay API, or per-game telemetry. The original binary's Direct3D imports show graphics capability, but they do not prove the exact original FPS implementation.

No DLL injection, anti-cheat bypass, credential access, persistence, security disabling, or license/trial bypass is included.
