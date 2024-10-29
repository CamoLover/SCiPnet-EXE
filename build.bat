@echo off
setlocal enabledelayedexpansion

color a

:: Get the directory where the batch file is located
set "SCRIPT_DIR=%~dp0"
cd /d "%SCRIPT_DIR%"

echo Building project in: %SCRIPT_DIR% 

:: Extract the project name from CMakeLists.txt
set "PROJECT_NAME="
for /f "usebackq tokens=2 delims=()" %%A in (`findstr /ri "^project(" "%SCRIPT_DIR%CMakeLists.txt"`) do (
    set "PROJECT_NAME=%%A"
)

set "PROJECT_NAME=%PROJECT_NAME: =%"
if not defined PROJECT_NAME (
    echo ERROR: Could not determine project name from CMakeLists.txt
    pause
    exit /b 1
)
echo Project name is: %PROJECT_NAME%

:: Create the build directory if it doesn't exist
if not exist "%SCRIPT_DIR%build" (
    echo Creating build directory...
    mkdir "%SCRIPT_DIR%build" 
        echo ERROR: Failed to create build directory.
        exit /b 1
    )
)

:: Navigate to the build directory
cd "%SCRIPT_DIR%build"

:: Run CMake to configure the project
echo Running CMake...
cmake ..

:: Build the project
echo Building the project...
cmake --build .

:: Locate and execute the executable
echo Attempting to locate the executable:
======================================================

set "EXE_PATH=%SCRIPT_DIR%build\\Debug\\%PROJECT_NAME%.exe"
if exist "%EXE_PATH%" (
    call "%EXE_PATH%" 
) else (
    echo ERROR: Could not locate executable %PROJECT_NAME%.exe in the build directory.
)

echo ======================================================

cd ..
pause
