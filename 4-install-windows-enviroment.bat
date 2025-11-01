@echo off
setlocal EnableExtensions
cd /d "%~dp0"

call tools\ensure_windows_tools.bat
if %errorlevel% neq 0 (
    echo Temel araclar kontrol edilirken hatalar olustu; devam ediliyor.
)

echo Checking administrative privileges...
net session >nul 2>&1
if %errorlevel%==0 (
    set "HAS_ADMIN=1"
) else (
    set "HAS_ADMIN=0"
)

if "%HAS_ADMIN%"=="1" (
    echo Installing/upgrading Chocolatey packages...
    choco upgrade astyle ninja cmake doxygen.install opencppcoverage lcov pandoc rsvg-convert python miktex curl marp-cli graphviz -y
) else (
    echo [warning] Chocolatey paketlerini yuklemek icin yonetici yetkisi gerekiyor.
    echo          Lutfen betigi "Run as administrator" olarak calistirin veya paketleri manuel kurun.
)

echo Installing/upgrading Python packages...
python -m pip install --upgrade --user coverxygen junit2html >nul
python -m pip install --upgrade --user mkdocs pymdown-extensions mkdocs-material mkdocs-material-extensions mkdocs-simple-hooks mkdocs-video mkdocs-minify-plugin mkdocs-git-revision-date-localized-plugin mkdocs-static-i18n mkdocs-with-pdf qrcode mkdocs-awesome-pages-plugin mkdocs-embed-external-markdown mkdocs-include-markdown-plugin mkdocs-ezlinks-plugin mkdocs-git-authors-plugin mkdocs-git-committers-plugin mkdocs-exclude pptx2md >nul

echo Downloading PlantUML jar...
curl -sL -o jq.exe https://github.com/stedolan/jq/releases/download/jq-1.6/jq-win64.exe
set "download_url="
for /f "delims=" %%A in ('curl -s https://api.github.com/repos/plantuml/plantuml/releases/latest ^| jq -r ".assets[] | select(.name | endswith(\"plantuml.jar\")) | .browser_download_url"') do (
    set "download_url=%%A"
)
if defined download_url (
    curl -sL -o plantuml.jar "%download_url%"
    echo PlantUML downloaded to %CD%\plantuml.jar
) else (
    echo PlantUML indirilemedi.
)
del jq.exe >nul 2>&1

pause
