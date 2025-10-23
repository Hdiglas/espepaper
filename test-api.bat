@echo off
echo ========================================
echo Backend API Testen
echo ========================================
echo.

echo Test 1: Health Check
curl http://localhost:3000/health
echo.
echo.

echo Test 2: Neuesten Text abrufen
curl http://localhost:3000/api/text/latest
echo.
echo.

echo Test 3: Alle Texte abrufen
curl http://localhost:3000/api/texts
echo.
echo.

pause

