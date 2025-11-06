@echo off
@setlocal enableextensions
@cd /d "%~dp0"

echo ========================================
echo Markdown to PDF Converter
echo ========================================
echo.

REM Check if pandoc is installed
where pandoc >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo Pandoc found! Converting MIDTERM_REPORT.md to PDF...
    pandoc MIDTERM_REPORT.md -o MIDTERM_REPORT.pdf --pdf-engine=pdflatex -V geometry:margin=2cm -V fontsize=11pt
    if %ERRORLEVEL% EQU 0 (
        echo.
        echo SUCCESS: MIDTERM_REPORT.pdf created!
        echo.
        goto :end
    ) else (
        echo.
        echo ERROR: Pandoc conversion failed. Trying alternative method...
        echo.
    )
)

REM Try with wkhtmltopdf if available
where wkhtmltopdf >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo wkhtmltopdf found! Converting via HTML...
    pandoc MIDTERM_REPORT.md -o temp_report.html -s --standalone
    if %ERRORLEVEL% EQU 0 (
        wkhtmltopdf temp_report.html MIDTERM_REPORT.pdf
        del temp_report.html
        if %ERRORLEVEL% EQU 0 (
            echo.
            echo SUCCESS: MIDTERM_REPORT.pdf created!
            echo.
            goto :end
        )
    )
)

REM If pandoc is not installed, provide instructions
echo Pandoc is not installed.
echo.
echo Please install Pandoc using one of the following methods:
echo.
echo Method 1: Using Chocolatey (Recommended)
echo   choco install pandoc
echo.
echo Method 2: Using Scoop
echo   scoop install pandoc
echo.
echo Method 3: Manual Installation
echo   Download from: https://pandoc.org/installing.html
echo.
echo After installing Pandoc, run this script again.
echo.
echo Alternatively, you can convert manually:
echo   1. Open MIDTERM_REPORT.md in a markdown viewer
echo   2. Print to PDF or use online converter
echo   3. Or use: pandoc MIDTERM_REPORT.md -o MIDTERM_REPORT.pdf
echo.

:end
pause

