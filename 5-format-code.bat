@echo off
setlocal EnableExtensions
cd /d "%~dp0"

echo Formatting repository sources with Astyle...

where astyle >nul 2>&1
if errorlevel 1 (
    echo [warning] Astyle not found in PATH. Skipping formatting.
    goto :end
)

set "ASTYLE_OPTS="
if exist "%~dp0astyle-options.txt" set "ASTYLE_OPTS=--options=%~dp0astyle-options.txt"

for %%D in (
    src\template
    src\inventory_app
    src\utility
    src\tests\template
    src\tests\utility
) do (
    if exist "%%D" (
        for /R "%%D" %%F in (*.h) do astyle --quiet %ASTYLE_OPTS% "%%F" >nul
        for /R "%%D" %%F in (*.hpp) do astyle --quiet %ASTYLE_OPTS% "%%F" >nul
        for /R "%%D" %%F in (*.c) do astyle --quiet %ASTYLE_OPTS% "%%F" >nul
        for /R "%%D" %%F in (*.cpp) do astyle --quiet %ASTYLE_OPTS% "%%F" >nul
        for /R "%%D" %%F in (*.cxx) do astyle --quiet %ASTYLE_OPTS% "%%F" >nul
        for /R "%%D" %%F in (*.orig) do del /f /q "%%F" >nul
    )
)

echo Formatting completed.

:end
pause
