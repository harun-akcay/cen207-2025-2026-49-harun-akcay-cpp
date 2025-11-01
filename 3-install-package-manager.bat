@echo off
setlocal EnableExtensions
cd /d "%~dp0"

echo Checking if Chocolatey is installed...
set "CHOC_PATH=%ProgramData%\Chocolatey\bin\choco.exe"
if exist "%CHOC_PATH%" (
    echo Chocolatey is already installed.
)
if not exist "%CHOC_PATH%" (
    echo Chocolatey is not installed. Please install it manually from https://chocolatey.org/install ^(administrator rights required^).
)

echo Checking if Scoop is installed...
set "SCOOP_FOUND="
for /f "delims=" %%I in ('where scoop 2^>nul') do set "SCOOP_FOUND=%%I"
if defined SCOOP_FOUND (
    echo Scoop is already installed.
) else (
    echo Scoop is not installed. Attempting installation for the current user...
    powershell -NoProfile -ExecutionPolicy Bypass -Command "iwr -useb get.scoop.sh | iex"
    if errorlevel 1 (
        echo Scoop installation failed. You can install it manually via PowerShell: iwr -useb get.scoop.sh | iex
    ) else (
        echo Scoop installed successfully.
    )
)

pause
