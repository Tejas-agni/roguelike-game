@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -no_logo
if %errorlevel% neq 0 (
    echo [ERROR] VsDevCmd.bat failed. C++ workload may not be installed.
    exit /b %errorlevel%
)
echo [OK] Developer environment loaded
cl.exe 2>&1 | findstr /C:"Microsoft"
if %errorlevel% neq 0 (
    echo [ERROR] cl.exe not found - C++ workload missing from Build Tools
    echo [INFO] Run the VS Installer and add the C++ build tools workload
    exit /b 1
)
echo [OK] cl.exe found, proceeding with build...
"C:\Program Files\CMake\bin\cmake.exe" -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 exit /b %errorlevel%
"C:\Program Files\CMake\bin\cmake.exe" --build build --config Release
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
echo [SUCCESS] Build complete: build\RoguelikeSurvival.exe
