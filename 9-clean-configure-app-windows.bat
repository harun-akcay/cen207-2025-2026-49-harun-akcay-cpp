@echo off
setlocal EnableExtensions
cd /d "%~dp0"

call tools\ensure_windows_tools.bat
if %errorlevel% neq 0 (
    echo Temizlik devam ediyor, ancak temel araclar bulunamadi.
)
if not defined CMAKE_CMD set "CMAKE_CMD=cmake"

echo clean project

for %%F in (
    "doxygen_lib_win.log"
    "doxygen_lib_linux.log"
    "doxygen_test_win.log"
    "doxygen_test_linux.log"
    "utility_tests_unit_win.cov"
    "inventory_lib_test_tests_unit_win.cov"
    "utility_tests_unit_linux.cov"
    "inventory_lib_test_tests_unit_linux.cov"
    "LastCoverageResults.log"
    "InventoryApp_unit_win_cobertura.xml"
    "coverage_linux.info"
    "CMakePresets.json"
) do (
    if exist %%F del /f /q %%F
)

for %%D in (
    ".vs"
    ".vscode"
    "out"
    "release"
    "publish"
    "build"
    "release_win"
    "publish_win"
    "build_win"
    "release_linux"
    "publish_linux"
    "build_linux"
    "docs\coverxygen"
    "docs\coveragereport"
    "docs\doxygen"
    "docs\coverxygenlibwin"
    "docs\coverxygentestwin"
    "docs\coveragereportlibwin"
    "docs\coveragereporttestwin"
    "docs\doxygenlibwin"
    "docs\doxygentestwin"
    "docs\coverxygenliblinux"
    "docs\coverxygentestlinux"
    "docs\coveragereportliblinux"
    "docs\coveragereporttestlinux"
    "docs\doxygenliblinux"
    "docs\doxygentestlinux"
    "docs\testresultswin"
    "docs\testresultslinux"
    "site"
) do (
    if exist %%D rd /S /Q %%D
)

echo Re-Configure CMAKE
call "%CMAKE_CMD%" -S . -B build_win -G "Visual Studio 17 2022"

pause
