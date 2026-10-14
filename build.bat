@echo off
REM One-click build. Run from a "x64 Native Tools Command Prompt for VS 2022",
REM or any prompt where cmake is on PATH and Visual Studio 2022 is installed.
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 || goto :err
cmake --build build --config Release || goto :err
mkdir dist 2>nul
copy /Y build\Release\ZAIDX_FPS_BOOSTER.exe dist\ZAIDX_FPS_BOOSTER.exe
echo.
echo Done: dist\ZAIDX_FPS_BOOSTER.exe
pause
exit /b 0
:err
echo Build failed.
pause
exit /b 1
