@echo off
@setlocal EnableExtensions
@cd /d "%~dp0"

echo [clean] Temizlik islemi baslatildi...

for %%F in (
    "doxygen_lib_win.log"
    "doxygen_lib_linux.log"
    "doxygen_test_win.log"
    "doxygen_test_linux.log"
    "*_tests_unit_win.cov"
    "*_tests_unit_linux.cov"
    "LastCoverageResults.log"
    "*_unit_win_cobertura.xml"
    "coverage_linux.info"
    "CMakePresets.json"
    "plantuml.jar"
    "tools\plantuml.jar"
    "release_win\*.tar.gz"
    "release_win\*.zip"
    "report_doc_*\*.xml"
    "report_test_*\*.xml"
) do (
    if exist "%%~F" del /f /q "%%~F" >nul 2>&1
)

for %%D in (
    ".vs"
    ".vscode"
    "out"
    "release"
    "publish"
    "build"
    "release_*"
    "publish_*"
    "build_*"
    "docs\coverxygen*"
    "docs\coveragereport*"
    "docs\doxygen*"
    "docs\testresults*"
    "docs\assets"
    "site"
) do (
    for /d %%E in (%%~D) do (
        if exist "%%~fE" rd /s /q "%%~fE" >nul 2>&1
    )
)

echo [clean] Temizlik islemi tamamlandi.
exit /b 0
