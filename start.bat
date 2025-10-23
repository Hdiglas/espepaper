@echo off
echo ========================================
echo E-Ink Display Backend Server
echo ========================================
echo.

REM Prüfen ob .env existiert
if not exist .env (
    echo Erstelle .env Datei...
    copy config.env .env
    echo .env Datei erstellt!
    echo.
)

REM Prüfen ob node_modules existiert
if not exist node_modules (
    echo Installiere Node.js Pakete...
    call npm install
    echo.
)

echo Starte Backend Server...
echo.
call npm start

