# ZAIDX FPS BOOSTER — Deep Recovery Phase 2

This is a clean-room native C++ reconstruction based on static analysis of the supplied executable.

## Phase 2 additions
- DXGI first-adapter GPU/vendor discovery retained.
- Windows power-plan friendly-name query added via PowrProf.
- Process list retained and a PID-targeted priority control was added.
- High/Normal priority controls are explicit and reversible.
- User TEMP cleanup remains limited to the current user's TEMP path.
- Authentication, trial/licensing checks, credential handling and unknown packed routines remain excluded.

## Why these features were selected
Static analysis showed imports associated with DirectX/DXGI, PDH/system monitoring, ShellExecute, file dialogs and Windows system APIs. Those imports support a graphics/performance utility interpretation, but they do not prove the original implementation. The reconstruction therefore implements only transparent, reversible functionality that can be justified without guessing hidden behavior.

## Accuracy
The original executable is heavily packed/obfuscated. Exact original source, UI layout, algorithms and hidden routines cannot be recovered from imports alone. This project is a functional clean-room rebuild, not a byte-for-byte source recovery.

## Safety boundary
Do not run the original unknown executable on a normal personal machine. Controlled unpacking/dynamic tracing should only be performed in a dedicated Windows reverse-engineering VM.
