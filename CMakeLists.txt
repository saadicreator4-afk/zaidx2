cmake_minimum_required(VERSION 3.20)
project(ZAIDX_FPS_BOOSTER LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ZAIDX_FPS_BOOSTER WIN32
    src/main.cpp
    src/dashboard.cpp
    src/gpu_info.cpp
    src/gpu_telemetry.cpp
    src/dashboard_window.cpp
    src/system_info.cpp
    src/optimizer.cpp
)
target_include_directories(ZAIDX_FPS_BOOSTER PRIVATE include)
target_compile_definitions(ZAIDX_FPS_BOOSTER PRIVATE UNICODE _UNICODE NOMINMAX)
target_link_libraries(ZAIDX_FPS_BOOSTER PRIVATE
    user32 gdi32 shell32 advapi32 psapi pdh dxgi d3d11 dwmapi powrprof comctl32 ole32 oleaut32
)
if(MINGW)
    # wWinMain entry point + fully static exe (no extra DLLs needed)
    target_link_options(ZAIDX_FPS_BOOSTER PRIVATE -municode -static -static-libgcc -static-libstdc++)
endif()
if(MSVC)
    # Static CRT so the exe runs without the VC++ redistributable
    set_property(TARGET ZAIDX_FPS_BOOSTER PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()
