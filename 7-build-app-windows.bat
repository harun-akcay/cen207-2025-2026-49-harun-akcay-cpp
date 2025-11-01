@echo off
@setlocal EnableExtensions
@cd /d "%~dp0"

call tools\ensure_windows_tools.bat
if %errorlevel% neq 0 (
    echo Zorunlu araclar yuklenemedi. Islemler durduruldu.
    goto :eof
)
if not defined CMAKE_CMD set "CMAKE_CMD=cmake"
if not defined CTEST_CMD set "CTEST_CMD=ctest"
if not defined DOXYGEN_CMD set "DOXYGEN_CMD=doxygen"
if not defined OPENCPPCOV_CMD set "OPENCPPCOV_CMD=OpenCppCoverage.exe"

rem Get the current directory path
for %%A in ("%~dp0.") do (
    set "currentDir=%%~fA"
)

::echo Clean Project
::call 9-clean-project.bat

echo Create the "release" folder and its contents
mkdir publish_win
mkdir release_win
mkdir build_win

echo Create the "docs" folder and its contents
mkdir docs
cd docs
mkdir coverxygenlibwin
mkdir coverxygentestwin
mkdir coveragereportlibwin
mkdir coveragereporttestwin
mkdir doxygenlibwin
mkdir doxygentestwin
mkdir testresultswin
cd ..

echo Create the "site" folder and its contents
mkdir site

echo Folders are Recreated successfully.

echo Generate Documentation

set STRIP_FROM_PATH=%currentDir%

echo Generate HTML/LATEX/RTF/XML Documentation for Library (No Source Code Only Headers)
call "%DOXYGEN_CMD%" DoxyfileLibWin

echo Generate HTML/LATEX/RTF/XML Documentation for Unit Tests (Test Sources and Test Data Sets)
call "%DOXYGEN_CMD%" DoxyfileTestWin

echo Not: coverxygen uses doxygen xml output for coverage

echo Run Documentation Coverage Data Collector for Library (No Source Code Only Headers)
call python -m coverxygen --xml-dir ./docs/doxygenlibwin/xml --src-dir ./ --format lcov --output ./docs/coverxygenlibwin/lcov_doxygen_lib_win.info

echo Run Documentation Coverage Data Collector for Unit Tests (Test Sources and Test Data Sets)
call python -m coverxygen --xml-dir ./docs/doxygentestwin/xml --src-dir ./ --format lcov --output ./docs/coverxygentestwin/lcov_doxygen_test_win.info
rem call python -m coverxygen --xml-dir ./docs/doxygen/xml --src-dir ./ --format lcov --output ./docs/coverxygen/lcov.info --prefix %currentDir%\

rem echo Run lcov genhtml
rem call perl C:\ProgramData\chocolatey\lib\lcov\tools\bin\genhtml --legend --title "Documentation Coverage Report" ./docs/coverxygen/lcov.info -o docs/coverxygen

echo Run Documentation Coverage Report Generator for Library 
call reportgenerator "-title:Project Inventory Library Documentation Coverage Report (Windows)" "-reports:**/lcov_doxygen_lib_win.info" "-targetdir:docs/coverxygenlibwin" "-reporttypes:Html" "-filefilters:-*.md;-*.xml;-*[generated];-*build*" "-historydir:report_doc_lib_hist_win"
call reportgenerator "-reports:**/lcov_doxygen_lib_win.info" "-targetdir:assets/doccoveragelibwin" "-reporttypes:Badges" "-filefilters:-*.md;-*.xml;-*[generated];-*build*"

echo Run Documentation Coverage Report Generator for Unit Tests 
call reportgenerator "-title:Project Inventory Test Documentation Coverage Report (Windows)" "-reports:**/lcov_doxygen_test_win.info" "-targetdir:docs/coverxygentestwin" "-reporttypes:Html" "-filefilters:-*.md;-*.xml;-*[generated];-*build*" "-historydir:report_doc_test_hist_win"
call reportgenerator "-reports:**/lcov_doxygen_test_win.info" "-targetdir:assets/doccoveragetestwin" "-reporttypes:Badges" "-filefilters:-*.md;-*.xml;-*[generated];-*build*"

echo Testing Application with Coverage
echo Configure CMAKE
call "%CMAKE_CMD%" -B build_win -DCMAKE_BUILD_TYPE=Debug -G "Visual Studio 17 2022" -DCMAKE_INSTALL_PREFIX:PATH=publish_win
echo Build CMAKE Debug/Release
call "%CMAKE_CMD%" --build build_win --config Debug -j4
call "%CMAKE_CMD%" --build build_win --config Release -j4
rem call "%CMAKE_CMD%" --install build_win --strip
start "Install Debug" "%CMAKE_CMD%" --install build_win --config Debug --strip
start "Install Release" "%CMAKE_CMD%" --install build_win --config Release --strip
echo Test CMAKE
cd build_win
:: Test are already run with OpenCppCoverage...
::call "%CTEST_CMD%" -C Debug -j4 --output-on-failure --output-log test_results_windows.log
call "%CTEST_CMD%" -C Debug -j4 --output-junit testResults_windows.xml --output-log test_results_windows.log
call python -m junit2html testResults_windows.xml testResults_windows.html
call copy testResults_windows.html "..\docs\testresultswin\index.html"
cd ..

echo Generate Test Coverage Data for Utility
call "%OPENCPPCOV_CMD%" --export_type=binary:utility_tests_unit_win.cov --sources src\utility\src --sources src\utility\header --sources src\tests\utility -- build_win\build\Debug\utility_tests.exe

echo Generate Test Coverage Data for Inventorymanager
call "%OPENCPPCOV_CMD%" --export_type=binary:inventory_lib_test_tests_unit_win.cov --sources src\inventory_lib\src --sources src\inventory_lib\header --sources src\tests\template -- build_win\build\Debug\inventory_lib_test_tests.exe

echo Generate Test Coverage Data for Simple Inventory Management for Crafters and Combine Results
call "%OPENCPPCOV_CMD%" --input_coverage=utility_tests_unit_win.cov --input_coverage=inventory_lib_test_tests_unit_win.cov --export_type=cobertura:InventoryApp_unit_win_cobertura.xml --sources src\utility\src --sources src\utility\header --sources src\inventory_lib\src --sources src\inventory_lib\header --sources src\inventory_app\src --sources src\inventory_app\header --sources src\tests\utility --sources src\tests\template -- build_win\build\Debug\InventoryApp.exe

echo Generate Unit Test Coverage Report
call reportgenerator "-title:Project Inventory Unit Test Coverage Report (Windows)" "-targetdir:docs/coveragereportlibwin" "-reporttypes:Html" "-reports:**/InventoryApp_unit_win_cobertura.xml" "-sourcedirs:src/utility/src;src/utility/header;src/inventory_lib/src;src/inventory_lib/header;src/inventory_app/src;src/inventory_app/header;src/tests/utility;src/tests/inventory_lib_test" "-filefilters:-*minkernel\*;-*gtest*;-*a_work*;-*gtest-*;-*gtest.cc;-*gtest.h;-*build*" "-historydir:report_test_hist_win"
call reportgenerator "-targetdir:assets/codecoveragelibwin" "-reporttypes:Badges" "-reports:**/InventoryApp_unit_win_cobertura.xml" "-sourcedirs:src/utility/src;src/utility/header;src/inventory_lib/src;src/inventory_lib/header;src/inventory_app/src;src/inventory_app/header;src/tests/utility;src/tests/inventory_lib_test" "-filefilters:-*minkernel\*;-*gtest*;-*a_work*;-*gtest-*;-*gtest.cc;-*gtest.h;-*build*"

echo Copy the "assets" folder and its contents to "docs" recursively
call robocopy assets "docs\assets" /E

echo Copy the "README.md" file to "docs\index.md"
call copy README.md "docs\index.md"

echo Files and folders copied successfully.

:: echo Generate Webpage
:: call mkdocs build

rem echo Publish Linux Binaries
rem call dotnet publish -c Release -r linux-x64 --self-contained true -o publish/linux

rem echo Publish MacOS Binaries
rem call dotnet publish -c Release -r osx-x64 --self-contained true -o publish/macos

rem echo Publish Windows Binaries
rem call dotnet publish -c Release -r win-x64 --self-contained true -o publish/windows

rem echo Package Linux Binaries
rem call tar -czvf release/linux-binaries.tar.gz -C publish/linux .

rem echo Package MacOS Binaries
rem call tar -czvf release/macos-binaries.tar.gz -C publish/macos .

echo Package Publish Windows Binaries
tar -czvf release_win\windows-publish-binaries.tar.gz -C publish_win .

echo Package Publish Windows Binaries
call robocopy src\utility\header "build_win\build\Release" /E
call robocopy src\inventory_lib\header "build_win\build\Release" /E
call robocopy src\inventory_app\header "build_win\build\Release" /E
tar -czvf release_win\windows-release-binaries.tar.gz -C build_win\build\Release .

echo Package Publish Debug Windows Binaries
call robocopy src\utility\header "build_win\build\Debug" /E
call robocopy src\inventory_lib\header "build_win\build\Debug" /E
call robocopy src\inventory_app\header "build_win\build\Debug" /E
tar -czvf release_win\windows-debug-binaries.tar.gz -C build_win\build\Debug .

echo Package Publish Test Coverage Report
tar -czvf release_win\windows-test-coverage-report.tar.gz -C docs\coveragereportlibwin .

echo Package Publish Library Doc Coverage Report
tar -czvf release_win\windows-lib-doc-coverage-report.tar.gz -C docs\coverxygenlibwin .

echo Package Publish Unit Test Doc Coverage Report
tar -czvf release_win\windows-test-doc-coverage-report.tar.gz -C docs\coverxygentestwin .

echo Package Publish Library Documentation
tar -czvf release_win\windows-doxygen-lib-documentation.tar.gz -C docs\doxygenlibwin .

echo Package Publish Unit Test Documentation
tar -czvf release_win\windows-doxygen-test-documentation.tar.gz -C docs\doxygentestwin .

echo Package Publish Test Results Report
tar -czvf release_win\windows-test-results-report.tar.gz -C docs\testresultswin .

echo ....................
echo Operation Completed!
echo ....................
pause
