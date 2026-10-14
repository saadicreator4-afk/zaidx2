# ZAIDX FPS BOOSTER — Phase 13

## Added
- Read-only GPU Engine activity using Windows PDH performance counters.
- GPU activity summed across available engine counters and capped at 100% for a simple dashboard scale.
- Unified CPU, RAM-used, and GPU activity history, with rolling 60 samples.
- Dashboard refreshes once per second while open.
- GPU counter availability is handled as optional; unsupported systems show GPU activity as unavailable.

## Limitations
- This is a GPU engine activity indicator, not a vendor-normalized guarantee of total GPU utilization. Engine counters can be absent or vary by Windows build and driver.
- GPU temperature is not included: temperature access is vendor/device-specific and cannot be reliably obtained through a universal DXGI API.
- This package was source-reviewed in the current environment but not compiled or runtime-tested on Windows here.

## Safety
No injection, anti-cheat bypass, memory modification, security disabling, persistence, driver manipulation, or license/trial bypass.
