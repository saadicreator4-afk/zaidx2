# ZAIDX FPS BOOSTER — Phase 3

Phase 3 expands the clean-room reconstruction without executing the original packed EXE.

## Added
- Enumerates all DXGI adapters instead of only the first GPU.
- Shows GPU vendor and dedicated VRAM.
- Adds a High Performance power-plan action that only activates an existing Windows plan named High performance or Ultimate Performance.
- Refreshes GPU/system/process information together.
- Keeps process priority controls and safe TEMP cleanup.

## Deliberately excluded
- Login/trial/license bypass.
- Credential/token extraction.
- Defender/security tampering.
- Persistence or startup installation.
- Driver manipulation.
- Execution of the original unknown packed executable.

## Evidence basis
The original binary imports Direct3D/DXGI, PDH, WinINet, ShellExecute, GUI/GDI components and process/system APIs. The Phase 3 additions focus on the high-confidence system/GPU/performance side of that evidence.

This remains a reconstruction, not recovered original source code.
