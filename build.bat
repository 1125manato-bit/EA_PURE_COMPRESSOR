@echo off
if not exist build mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build . --config Release --parallel 4
if %errorlevel% neq 0 exit /b %errorlevel%
