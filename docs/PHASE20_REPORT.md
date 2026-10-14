# Phase 20 — Process Controls and Telemetry Retry Report

## Process selection
- Corrected the PID parser's C++ null-terminator character literal.
- Normal-priority action now rejects an empty selection with a clear log message.

## GPU telemetry
- PDH query/counter initialization failures clear the initialization state so later timer samples can retry.
- Runtime data-collection failures report unavailable telemetry for that sample without crashing.

## System info
- Logical processor count has a defensive fallback if Windows reports zero.

## Validation status
Targeted source assertions and ZIP integrity are checked. Windows compile/runtime tests remain pending.
