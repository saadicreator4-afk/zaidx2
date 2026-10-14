# Build

Requirements:
- Windows 10/11
- Visual Studio 2022 with Desktop C++ workload
- CMake 3.20+

Commands:
```text
cmake -S . -B build
cmake --build build --config Release
```

The output executable is a clean rebuild and does not contain the original login/trial mechanism.
