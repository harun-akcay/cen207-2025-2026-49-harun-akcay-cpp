@echo off
setlocal EnableExtensions EnableDelayedExpansion
cd /d "%~dp0"

echo ::: INIT SUBMODULES BEGIN :::

for /f "delims=" %%I in ('git ls-files --full-name -- "desktop.ini" 2^>nul') do (
    git rm --cached --force "%%I" >nul 2>&1
)
for /f "delims=" %%I in ('dir /s /b /a:-d "desktop.ini" 2^>nul') do (
    del /f /q "%%I" >nul 2>&1
)

git submodule update --init --recursive
if errorlevel 1 (
    echo Submodule update failed. Please ensure submodules do not contain local changes.
)

echo ::: INIT SUBMODULES COMPLETED :::
pause
