@echo off
echo Cleaning project...
del "raycast.exe"
rd /s /q "./object"
mkdir "./object"
echo Cleaning complete!
echo Rebuilding project...
mingw32-make
echo Rebuild complete!
pause