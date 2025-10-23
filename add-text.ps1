# PowerShell Script um neuen Text zur Datenbank hinzuzufuegen

param(
    [Parameter(Mandatory=$true)]
    [string]$Text
)

$url = "http://localhost:3000/api/text"
$body = @{
    content = $Text
} | ConvertTo-Json

try {
    $response = Invoke-RestMethod -Uri $url -Method POST -ContentType "application/json" -Body $body
    
    if ($response.success) {
        Write-Host "[OK] Text erfolgreich hinzugefuegt!" -ForegroundColor Green
        Write-Host "ID: $($response.text.id)"
        Write-Host "Inhalt: $($response.text.content)"
        Write-Host "Erstellt: $($response.text.created_at)"
    } else {
        Write-Host "[FEHLER] Fehler beim Hinzufuegen" -ForegroundColor Red
    }
} catch {
    Write-Host "[FEHLER] Verbindungsfehler: $_" -ForegroundColor Red
}

