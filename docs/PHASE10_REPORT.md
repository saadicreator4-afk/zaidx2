# ZAIDX FPS BOOSTER — Phase 10

Phase 10 introduces the foundation for a lightweight performance-history dashboard.

## Added
- `PerfSample` data structure for CPU, RAM and thread telemetry.
- Rolling history container capped at 60 samples.
- Clear-history support in the dashboard layer.
- Existing safe Gaming Mode and process telemetry remain.

## Safety
The history layer is local/read-only telemetry. It does not inject into games, modify game memory, bypass anti-cheat, disable security, create persistence, or bypass licensing.

## FPS
No fabricated FPS values are shown. A real FPS counter requires trustworthy rendering telemetry or a supported game/overlay integration.
