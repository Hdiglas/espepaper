# E-Ink Display mit PostgreSQL Anbindung

Dieses Projekt verbindet einen ESP32 mit einem Waveshare 7.5" E-Ink Display, um Texte aus einer PostgreSQL-Datenbank anzuzeigen.

## 🎯 Quick Start

**Für den Schnellstart siehe:** [`SCHNELLSTART.md`](SCHNELLSTART.md)

**Für detaillierte Installation siehe:** [`INSTALLATION.md`](INSTALLATION.md)

## Architektur

- **ESP32**: Verbindet sich mit WLAN und ruft Texte über HTTP ab
- **Backend API**: Node.js Server, der die PostgreSQL-Datenbank abfragt
- **PostgreSQL**: Externe Datenbank mit Texten

## Hardware

- ESP32 Development Board
- Waveshare 7.5" E-Ink Display V2 (B/W)

## Konfiguration

### WLAN
- **SSID**: work
- **Passwort**: letmeinplz!

### PostgreSQL-Datenbank
- **Host**: diglas.ddns.net
- **Port**: 8564
- **Datenbank**: appdb
- **Benutzer**: User198738

### Backend-API
- **Port**: 3000
- **Endpoint**: `/api/text/latest`

## Installation (Kurzversion)

### 1. Backend starten

```powershell
cd backend
.\start.bat
```

### 2. ESP32 programmieren

1. Öffne `esp32_eink_display/esp32_eink_display.ino` in Arduino IDE
2. Passe die Backend-IP-Adresse an
3. Installiere erforderliche Bibliotheken (siehe unten)
4. Lade den Code auf den ESP32 hoch

### Erforderliche Arduino-Bibliotheken

- WiFi (Standard ESP32 Bibliothek)
- HTTPClient (Standard ESP32 Bibliothek)
- ArduinoJson (über Library Manager installieren)
- Waveshare E-Paper Bibliothek (https://github.com/waveshare/e-Paper)

## Konfiguration

### WLAN-Zugangsdaten
- SSID: work
- Passwort: letmeinplz!

### PostgreSQL-Datenbank
- Host: diglas.ddns.net
- Port: 8564
- Datenbank: appdb
- Benutzer: User198738
- Passwort: sadfjkhk908ewrsdkfhjk

### Backend-API URL
Standardmäßig: `http://localhost:3000`
Für ESP32: IP-Adresse des Rechners, auf dem das Backend läuft

## Datenbankstruktur

Die PostgreSQL-Datenbank sollte eine Tabelle mit Texten enthalten. Beispiel:

```sql
CREATE TABLE texts (
    id SERIAL PRIMARY KEY,
    content TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

INSERT INTO texts (content) VALUES ('Hallo das ist Ein Test');
```

## Verwendung

1. Starte das Backend
2. Der ESP32 verbindet sich automatisch mit WLAN
3. Ruft alle 60 Sekunden neue Texte aus der Datenbank ab
4. Zeigt den neuesten Text auf dem E-Ink Display an

