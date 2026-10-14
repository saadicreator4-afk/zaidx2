# Phase 17 — Process Safety and Robustness Report

## Process priority protections
- PID 0 is rejected.
- PID 4 (Windows System) is rejected.
- The selected-process priority API rejects the ZAIDX process itself; the dedicated ZAIDX Priority button remains separate.
- The opened handle's process ID is checked before applying a priority change.
- Failure reports include the Windows error code.

## User confirmation
The High button and Gaming Mode now ask before changing the selected process to High priority. Choosing No cancels the action.

## Telemetry robustness
- Added explicit `<cwchar>` / `<cwctype>` includes for wide-character functions.
- Process CPU sampling resets safely if counters appear to move backwards.

## Validation status
Source-level checks and archive integrity are performed. Windows compile/runtime tests remain pending.
