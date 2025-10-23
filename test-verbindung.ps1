# Test-Script für ESP32 E-Ink Display Verbindung
# Prüft ob Backend erreichbar ist

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "E-Ink Display - Verbindungstest" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 1. IP-Adresse anzeigen
Write-Host "[1/5] Lokale IP-Adresse:" -ForegroundColor Yellow
$ip = (Get-NetIPAddress -AddressFamily IPv4 | Where-Object {$_.InterfaceAlias -notlike "*Loopback*" -and $_.IPAddress -notlike "169.254.*"} | Select-Object -First 1).IPAddress
Write-Host "      $ip" -ForegroundColor Green
Write-Host ""

# 2. Backend-URL
$backendUrl = "http://${ip}:3000"
Write-Host "[2/5] Backend-URL:" -ForegroundColor Yellow
Write-Host "      $backendUrl" -ForegroundColor Green
Write-Host ""

# 3. Port-Test
Write-Host "[3/5] Prüfe ob Port 3000 offen ist..." -ForegroundColor Yellow
$portTest = Test-NetConnection -ComputerName $ip -Port 3000 -WarningAction SilentlyContinue
if ($portTest.TcpTestSucceeded) {
    Write-Host "      ✓ Port 3000 ist erreichbar!" -ForegroundColor Green
} else {
    Write-Host "      ✗ Port 3000 ist NICHT erreichbar!" -ForegroundColor Red
    Write-Host "      Backend läuft möglicherweise nicht." -ForegroundColor Red
}
Write-Host ""

# 4. Health-Check
Write-Host "[4/5] Teste Backend Health-Endpoint..." -ForegroundColor Yellow
try {
    $health = Invoke-RestMethod -Uri "$backendUrl/health" -Method GET -TimeoutSec 5
    if ($health.status -eq "OK") {
        Write-Host "      ✓ Backend antwortet: OK" -ForegroundColor Green
    }
} catch {
    Write-Host "      ✗ Backend antwortet NICHT!" -ForegroundColor Red
    Write-Host "      Fehler: $_" -ForegroundColor Red
}
Write-Host ""

# 5. Text-API Test
Write-Host "[5/5] Teste Text-API..." -ForegroundColor Yellow
try {
    $response = Invoke-RestMethod -Uri "$backendUrl/api/text/latest" -Method GET -TimeoutSec 5
    if ($response.success) {
        Write-Host "      ✓ API funktioniert!" -ForegroundColor Green
        Write-Host "      Text: $($response.text)" -ForegroundColor Cyan
    } else {
        Write-Host "      ✗ API meldet Fehler" -ForegroundColor Red
    }
} catch {
    Write-Host "      ✗ Text-API antwortet NICHT!" -ForegroundColor Red
    Write-Host "      Fehler: $_" -ForegroundColor Red
}
Write-Host ""

# Zusammenfassung
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "ESP32 Konfiguration:" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Trage diese URL im ESP32-Code ein:" -ForegroundColor Yellow
Write-Host ""
Write-Host "const char* apiUrl = `"$backendUrl/api/text/latest`";" -ForegroundColor Green
Write-Host ""
Write-Host "Die aktuelle IP ist: $ip" -ForegroundColor Yellow
Write-Host ""

