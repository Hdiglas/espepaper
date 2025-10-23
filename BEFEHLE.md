# Nützliche Befehle

## Backend

### Server starten
```powershell
cd backend
.\start.bat
```

Oder manuell:
```powershell
cd backend
npm start
```

### Server stoppen
```powershell
Get-Process -Name node | Stop-Process -Force
```

### API testen
```powershell
# Health Check
Invoke-RestMethod -Uri "http://localhost:3000/health"

# Neuesten Text abrufen
Invoke-RestMethod -Uri "http://localhost:3000/api/text/latest"

# Alle Texte abrufen
Invoke-RestMethod -Uri "http://localhost:3000/api/texts"
```

### Neuen Text hinzufügen

Via PowerShell-Script:
```powershell
cd backend
.\add-text.ps1 "Mein neuer Text"
```

Oder direkt:
```powershell
Invoke-RestMethod -Uri "http://localhost:3000/api/text" -Method POST -ContentType "application/json" -Body '{"content":"Mein Text"}'
```

## PostgreSQL

### Mit Datenbank verbinden
```bash
psql -h diglas.ddns.net -p 8564 -U User198738 -d appdb
```

### Texte anzeigen
```sql
SELECT * FROM texts ORDER BY created_at DESC;
```

### Neuen Text einfügen
```sql
INSERT INTO texts (content) VALUES ('Dein Text hier');
```

### Text löschen
```sql
DELETE FROM texts WHERE id = 1;
```

### Alle Texte löschen
```sql
TRUNCATE TABLE texts;
```

## Netzwerk

### IP-Adresse anzeigen
```powershell
ipconfig
```
Suche nach "IPv4-Adresse"

### Port prüfen (ob Backend läuft)
```powershell
Test-NetConnection -ComputerName localhost -Port 3000
```

### Firewall-Regel für Port 3000 erstellen
```powershell
New-NetFirewallRule -DisplayName "Backend API" -Direction Inbound -LocalPort 3000 -Protocol TCP -Action Allow
```

## ESP32

### Seriellen Monitor öffnen
In Arduino IDE: **Werkzeuge → Serieller Monitor** (Strg+Shift+M)

Baudrate: **115200**

### Code kompilieren ohne hochladen
In Arduino IDE: **Sketch → Verifizieren/Kompilieren** (Strg+R)

### Code hochladen
In Arduino IDE: **Sketch → Hochladen** (Strg+U)

## Debugging

### Backend-Logs live anzeigen
```powershell
cd backend
npm start
# Logs erscheinen direkt im Terminal
```

### Prüfen ob Datenbank erreichbar ist
```powershell
Test-NetConnection -ComputerName diglas.ddns.net -Port 8564
```

### Alle laufenden Node-Prozesse anzeigen
```powershell
Get-Process -Name node
```

