# Phase 19 — Process and Telemetry Defensive Checks

## Process list and selection
- Process selection validates the control, selected index, text retrieval, and numeric PID syntax.
- Process list insertion failures are handled without assuming an item was added.
- Restoring selection uses the listbox's returned item index.

## CPU telemetry
- Resets the sampling baseline if FILETIME counters move backwards.
- Rejects impossible idle deltas and clamps calculated CPU usage to the 0–100% range.

## Dashboard
- A null history pointer is rejected at dashboard creation.
- Paint handler renders a fallback message rather than dereferencing a null pointer.

## Validation status
Targeted source assertions and ZIP integrity are checked. Windows compile/runtime tests remain pending.
