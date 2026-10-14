# ZAIDX FPS BOOSTER — Phase 5

Phase 5 adds a safer process-focused workflow.

## Added
- Selected-process CPU affinity mask inspection.
- Explicit process targeting before priority changes.
- Better separation between system monitoring and target-process actions.
- Existing live CPU, RAM, GPU, process, TEMP cleanup and power-plan functionality retained.

## Safety boundary
This phase does **not** automatically change CPU affinity, terminate processes, disable security software, alter registry policies, install persistence, manipulate drivers, access credentials/tokens, or bypass login/trial/license controls.

The affinity feature is intentionally read-only: it reports the target process's current affinity mask so a future UI can offer a reversible, user-confirmed setting if desired.

This remains a clean-room reconstruction, not recovered original source.
