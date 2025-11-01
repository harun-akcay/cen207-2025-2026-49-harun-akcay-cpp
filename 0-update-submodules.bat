@echo off
setlocal EnableExtensions EnableDelayedExpansion
cd /d "%~dp0"

echo ::: UPDATE SUBMODULES BEGIN :::

for /f "delims=" %%I in ('git ls-files --full-name -- "desktop.ini" 2^>nul') do (
    git rm --cached --force "%%I" >nul 2>&1
)
for /f "delims=" %%I in ('dir /s /b /a:-d "desktop.ini" 2^>nul') do (
    del /f /q "%%I" >nul 2>&1
)

git submodule update --remote --merge
if errorlevel 1 (
    echo Submodule update failed. Please ensure submodules do not contain local changes.
)

echo ::: UPDATE SUBMODULES COMPLETED :::
pause
