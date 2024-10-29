@echo off
setlocal

REM Set variables for SFML version and directories
set "SFML_VERSION=2.6.1"
set "SFML_DIR=SFML"
set "SFML_URL=https://www.sfml-dev.org/files/SFML-%SFML_VERSION%-windows-vc17-64-bit.zip"
set "ZIP_FILE=%SFML_DIR%\SFML-%SFML_VERSION%.zip"

REM Create the SFML directory if it doesn't exist
if not exist "%SFML_DIR%" (
    mkdir "%SFML_DIR%"
)

REM Download SFML using PowerShell
echo Downloading SFML...
powershell -Command "Invoke-WebRequest -Uri '%SFML_URL%' -OutFile '%ZIP_FILE%'"
if %errorlevel% neq 0 (
    echo "Failed to download SFML."
    exit /b 1
)

REM Verify the download
if not exist "%ZIP_FILE%" (
    echo "Downloaded file not found."
    exit /b 1
)

REM Extract the downloaded zip file
echo Extracting SFML...
powershell -Command "Expand-Archive -Path '%ZIP_FILE%' -DestinationPath '%SFML_DIR%' -Force"
if %errorlevel% neq 0 (
    echo "Failed to extract SFML."
    exit /b 1
)

REM Clean up zip file
del "%ZIP_FILE%"

echo SFML installation completed successfully!
endlocal
exit /b 0
