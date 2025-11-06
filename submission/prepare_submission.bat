@echo off
@setlocal enableextensions
@cd /d "%~dp0"

echo ========================================
echo Prepare Project Submission Package
echo ========================================
echo.

REM Create submission folder
echo Creating submission folder...
if exist "submission" (
    echo Removing old submission folder...
    rd /S /Q "submission"
)
mkdir submission

REM Copy all tracked files (excluding ignored files)
echo Copying project files...
echo.

REM Copy source files
echo Copying source files...
xcopy /E /I /Y "src" "submission\src"

REM Copy documentation files (excluding doxygen)
echo Copying documentation files...
xcopy /E /I /Y "docs" "submission\docs" /EXCLUDE:exclude_doxygen.txt
if not exist "submission\docs" mkdir "submission\docs"
copy /Y "docs\index.md" "submission\docs\index.md" 2>nul
copy /Y "docs\architecture.md" "submission\docs\architecture.md" 2>nul
copy /Y "docs\security.md" "submission\docs\security.md" 2>nul
copy /Y "docs\developers.md" "submission\docs\developers.md" 2>nul

REM Copy configuration files
echo Copying configuration files...
copy /Y "CMakeLists.txt" "submission\CMakeLists.txt"
copy /Y "README.md" "submission\README.md"
copy /Y "mkdocs.yml" "submission\mkdocs.yml"
copy /Y ".gitignore" "submission\.gitignore"

REM Copy Doxygen configuration files (but not generated docs)
echo Copying Doxygen configuration files...
copy /Y "DoxyfileLibWin" "submission\DoxyfileLibWin"
copy /Y "DoxyfileLibLinux" "submission\DoxyfileLibLinux"
copy /Y "DoxyfileTestWin" "submission\DoxyfileTestWin"
copy /Y "DoxyfileTestLinux" "submission\DoxyfileTestLinux"

REM Copy build scripts
echo Copying build scripts...
copy /Y "*.bat" "submission\" 2>nul
copy /Y "*.sh" "submission\" 2>nul
copy /Y "*.txt" "submission\" 2>nul

REM Copy report files
echo Copying report files...
copy /Y "MIDTERM_REPORT.md" "submission\MIDTERM_REPORT.md"
copy /Y "COMPLEXITY_ANALYSIS.md" "submission\COMPLEXITY_ANALYSIS.md"
copy /Y "C_IMPLEMENTATION_STATUS.md" "submission\C_IMPLEMENTATION_STATUS.md"
copy /Y "PROJECT_ANALYSIS.md" "submission\PROJECT_ANALYSIS.md" 2>nul
if exist "Midterm Project Report - CEN207.pdf" (
    copy /Y "Midterm Project Report - CEN207.pdf" "submission\Midterm Project Report - CEN207.pdf"
)

REM Copy GitHub workflow
echo Copying GitHub workflow...
if exist ".github" (
    xcopy /E /I /Y ".github" "submission\.github"
)

REM Copy assets (excluding generated coverage badges)
echo Copying assets...
if exist "assets" (
    xcopy /E /I /Y "assets" "submission\assets"
)

REM Remove doxygen generated files from submission
echo Removing Doxygen generated files from submission...
if exist "submission\docs\doxygenlibwin" rd /S /Q "submission\docs\doxygenlibwin"
if exist "submission\docs\doxygentestwin" rd /S /Q "submission\docs\doxygentestwin"
if exist "submission\docs\coverxygenlibwin" rd /S /Q "submission\docs\coverxygenlibwin"
if exist "submission\docs\coverxygentestwin" rd /S /Q "submission\docs\coverxygentestwin"
if exist "submission\docs\coveragereportlibwin" rd /S /Q "submission\docs\coveragereportlibwin"
if exist "submission\docs\coveragereporttestwin" rd /S /Q "submission\docs\coveragereporttestwin"

REM Remove build outputs
echo Removing build outputs...
if exist "submission\build" rd /S /Q "submission\build"
if exist "submission\build_win" rd /S /Q "submission\build_win"
if exist "submission\build_linux" rd /S /Q "submission\build_linux"
if exist "submission\release_win" rd /S /Q "submission\release_win"
if exist "submission\release_linux" rd /S /Q "submission\release_linux"
if exist "submission\publish_win" rd /S /Q "submission\publish_win"
if exist "submission\publish_linux" rd /S /Q "submission\publish_linux"

REM Remove binary files
echo Removing binary files...
del /Q /F "submission\*.bin" 2>nul
del /Q /F "submission\*.cov" 2>nul
del /Q /F "submission\*.cobertura.xml" 2>nul
del /Q /F "submission\cobertura.xml" 2>nul

REM Create zip file
echo.
echo Creating zip file...
if exist "cen207-midterm-submission.zip" del /Q /F "cen207-midterm-submission.zip"

REM Use PowerShell to create zip (more reliable)
powershell -Command "Compress-Archive -Path 'submission\*' -DestinationPath 'cen207-midterm-submission.zip' -Force"

echo.
echo ========================================
echo Submission Package Created!
echo ========================================
echo.
echo Zip file: cen207-midterm-submission.zip
echo Submission folder: submission\
echo.
echo IMPORTANT:
echo - Doxygen files are NOT included (as required)
echo - Binary files are NOT included
echo - Build outputs are NOT included
echo - Only source code and documentation are included
echo.
echo Ready for submission!
echo.
pause

