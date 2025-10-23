# Script zum Pruefen des Servers und der aktiven Texte

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "  E-Ink Display - Server Status" -ForegroundColor Cyan
Write-Host "========================================`n" -ForegroundColor Cyan

# Server Health Check
try {
    $health = Invoke-RestMethod -Uri "http://localhost:3000/health" -TimeoutSec 3
    Write-Host "[OK] Server laeuft" -ForegroundColor Green
} catch {
    Write-Host "[FEHLER] Server nicht erreichbar!" -ForegroundColor Red
    Write-Host "Bitte starte den Server mit: .\backend\start.bat" -ForegroundColor Yellow
    exit
}

# Aktive Texte abrufen
try {
    $response = Invoke-RestMethod -Uri "http://localhost:3000/api/text/latest" -TimeoutSec 3
    Write-Host "`n[INFO] Anzahl aktiver Texte: $($response.count) von 7 moeglich" -ForegroundColor Cyan
    Write-Host "`nAktive Texte auf dem Display:`n" -ForegroundColor White
    
    for($i=0; $i -lt $response.texts.Count; $i++) {
        Write-Host "  $($i+1). $($response.texts[$i])" -ForegroundColor White
    }
    
    Write-Host "`n[INFO] ESP32 ruft diese URL ab:" -ForegroundColor Cyan
    Write-Host "  http://192.168.0.162:3000/api/text/latest" -ForegroundColor Yellow
    Write-Host "`n[INFO] ESP32 Update-Intervall: alle 60 Sekunden" -ForegroundColor Cyan
    
} catch {
    Write-Host "[FEHLER] Konnte Texte nicht abrufen: $_" -ForegroundColor Red
}

Write-Host "`n========================================`n" -ForegroundColor Cyan

