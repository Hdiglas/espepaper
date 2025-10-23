# Fehlerbehebung

## Backend-Probleme

### ❌ "Port 3000 already in use"

**Problem:** Ein anderer Prozess nutzt bereits Port 3000.

**Lösung 1:** Stoppe den laufenden Node-Prozess
```powershell
Get-Process -Name node | Stop-Process -Force
```

**Lösung 2:** Ändere den Port in `backend/config.env`
```
PORT=3001
```

### ❌ "Fehler bei der Verbindung zur Datenbank"

**Mögliche Ursachen:**

1. **Datenbank ist nicht erreichbar**
   ```powershell
   Test-NetConnection -ComputerName diglas.ddns.net -Port 8564
   ```
   
2. **Falsche Zugangsdaten**
   - Prüfe `backend/.env` oder `backend/config.env`
   - Stelle sicher, dass die Werte korrekt sind

3. **Firewall blockiert Verbindung**
   - Prüfe Windows Firewall
   - Prüfe Router-Einstellungen

### ❌ "Cannot find module 'express'"

**Problem:** Node-Module nicht installiert.

**Lösung:**
```powershell
cd backend
npm install
```

## ESP32-Probleme

### ❌ ESP32 verbindet sich nicht mit WLAN

**Lösungen:**

1. **Prüfe SSID und Passwort**
   ```cpp
   const char* ssid = "work";
   const char* password = "letmeinplz!";
   ```

2. **Stelle sicher, dass es ein 2.4 GHz WLAN ist**
   - ESP32 unterstützt KEIN 5 GHz

3. **Prüfe WLAN-Reichweite**
   - ESP32 sollte in der Nähe des Routers sein

4. **Serieller Monitor zeigt Fehler**
   ```
   WLAN-Verbindung fehlgeschlagen!
   ```
   → Prüfe Zugangsdaten und Reichweite

### ❌ "HTTP-Fehler: -1" oder "HTTP-Fehler: 404"

**Problem:** Backend-API nicht erreichbar.

**Lösungen:**

1. **Prüfe Backend-IP-Adresse im Code**
   ```cpp
   const char* apiUrl = "http://192.168.1.100:3000/api/text/latest";
   ```
   
2. **Stelle sicher, dass Backend läuft**
   ```powershell
   curl http://localhost:3000/health
   ```

3. **ESP32 und Backend im selben Netzwerk?**
   - Beide müssen im gleichen WLAN sein
   
4. **Windows Firewall blockiert?**
   - Erlaube eingehende Verbindungen auf Port 3000

### ❌ Display zeigt nichts an

**Lösungen:**

1. **Prüfe Verkabelung**
   - SPI-Verbindungen korrekt?
   - Stromversorgung ausreichend?

2. **Prüfe Bibliotheken**
   - Waveshare E-Paper Bibliothek installiert?
   - Korrekte Version für 7.5" V2?

3. **Initialisierung im Seriellen Monitor prüfen**
   ```
   Initialisiere E-Ink Display...
   Display initialisiert!
   ```

### ❌ "Fehler: Speicher für Bildpuffer konnte nicht allokiert werden"

**Problem:** ESP32 hat nicht genug RAM.

**Lösungen:**

1. **Verwende Board mit mehr RAM** (ESP32-WROVER)

2. **Oder reduziere die Bildgröße** (nicht empfohlen für dieses Display)

### ❌ Kompilierungsfehler

**Häufige Fehler:**

1. **"DEV_Config.h: No such file or directory"**
   → Waveshare-Bibliothek nicht installiert

2. **"WiFi.h: No such file or directory"**
   → ESP32 Board nicht im Board-Manager installiert

3. **"ArduinoJson.h: No such file or directory"**
   → ArduinoJson-Bibliothek nicht installiert

## Netzwerk-Probleme

### ❌ Backend von anderem Gerät nicht erreichbar

**Lösungen:**

1. **Windows Firewall-Regel erstellen**
   ```powershell
   New-NetFirewallRule -DisplayName "Backend API" -Direction Inbound -LocalPort 3000 -Protocol TCP -Action Allow
   ```

2. **IP-Adresse prüfen**
   ```powershell
   ipconfig
   ```
   Verwende die IPv4-Adresse (nicht 127.0.0.1)

3. **Backend mit 0.0.0.0 starten** (akzeptiert alle Verbindungen)
   - Im Code ändern: `app.listen(port, '0.0.0.0')`

### ❌ "JSON-Parse-Fehler"

**Problem:** API-Antwort ist kein gültiges JSON.

**Lösung:**
- Prüfe Backend-Logs
- Teste API manuell:
  ```powershell
  Invoke-RestMethod -Uri "http://localhost:3000/api/text/latest"
  ```

## PostgreSQL-Probleme

### ❌ "Tabelle texts existiert nicht"

**Problem:** Tabelle wurde nicht erstellt.

**Lösung:**
Das aktualisierte Backend erstellt die Tabelle automatisch. Falls nicht:

```sql
CREATE TABLE texts (
    id SERIAL PRIMARY KEY,
    content TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

### ❌ "Connection refused"

**Problem:** PostgreSQL-Server nicht erreichbar.

**Lösungen:**

1. **Prüfe Verbindung**
   ```powershell
   Test-NetConnection -ComputerName diglas.ddns.net -Port 8564
   ```

2. **Prüfe DNS**
   ```powershell
   nslookup diglas.ddns.net
   ```

## Allgemeine Tipps

### Debug-Modus aktivieren

**ESP32:**
- Seriellen Monitor öffnen (115200 Baud)
- Alle Debug-Ausgaben werden angezeigt

**Backend:**
- Läuft bereits im Debug-Modus
- Alle Anfragen werden geloggt

### Neustart

Manchmal hilft ein kompletter Neustart:

1. **ESP32:** Reset-Button drücken
2. **Backend:** Stoppen und neu starten
3. **Router:** Neu starten (bei WLAN-Problemen)

