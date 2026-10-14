# ZAIDX FPS BOOSTER — Phase 14

## Dashboard correctness and reliability
- RAM history is now plotted as a percentage of physical memory, matching the graph's 0–100% axis; MB used remains visible in the live status line.
- Added the missing Clear History button action so it clears the rolling 60-sample history.
- Updated the main-window phase label to Phase 14.
- Preserved optional GPU Engine activity counters and existing process/system monitoring.

## Notes and limitations
- GPU engine activity remains an aggregate activity indicator, not a vendor-normalized total GPU utilization reading.
- GPU temperature remains unavailable through universal DXGI APIs and is not claimed by this phase.
- Windows compilation/runtime testing was not available in this build environment. Please build and test on Windows with Visual Studio 2022 and CMake.

## Safety
Read-only telemetry where possible. Existing explicit priority/cleanup controls remain user-triggered and reversible where supported. No injection, anti-cheat bypass, memory modification, security disabling, persistence, driver manipulation, or license bypass.
