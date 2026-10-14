# Phase 18 — Dashboard and Telemetry Reliability Report

## Dashboard lifecycle
- A single dashboard window is reused when the user presses Performance Graph multiple times.
- Existing dashboard is restored, focused, and invalidated for repaint.
- The global dashboard handle is cleared on window destruction.

## Graph robustness
- Stored CPU, RAM percentage, and GPU percentage values are clamped to 0–100.
- Subtitle rendering uses the actual wide-string length instead of a numeric literal.

## GPU telemetry initialization
- PDH query handles are closed when counter expansion or baseline collection fails.
- Added explicit `<cwchar>` include for wide-character string functions.

## Validation status
Targeted source assertions and ZIP integrity are checked. Windows compile/runtime tests remain pending.
