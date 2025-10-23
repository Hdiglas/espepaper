# Projektstruktur

```
Teseinkscreen postgres for all/
│
├── README.md                      # Hauptdokumentation
├── SCHNELLSTART.md               # Quick-Start Anleitung
├── INSTALLATION.md               # Detaillierte Installation
├── PROJEKTSTRUKTUR.md            # Diese Datei
├── .gitignore                    # Git Ignore Datei
│
├── backend/                      # Node.js Backend-API
│   ├── package.json              # Node.js Abhängigkeiten
│   ├── server.js                 # Haupt-Server-Datei
│   ├── config.env                # Konfigurations-Template
│   ├── .gitignore                # Backend Git Ignore
│   ├── start.bat                 # Windows Start-Script
│   ├── test-api.bat              # API Test-Script
│   └── add-text.ps1              # PowerShell Script zum Texte hinzufügen
│
├── esp32_eink_display/           # ESP32 Arduino Code
│   └── esp32_eink_display.ino    # Haupt-Sketch für ESP32
│
└── database/                     # Datenbank-Scripts
    └── setup.sql                 # PostgreSQL Setup-Script
```

## Komponenten-Beschreibung

### Backend (Node.js + Express)
- **server.js**: REST-API Server
  - GET `/api/text/latest` - Neuesten Text abrufen
  - GET `/api/texts` - Alle Texte abrufen
  - POST `/api/text` - Neuen Text hinzufügen
  - GET `/health` - Server-Status prüfen

### ESP32 Sketch
- **WLAN-Verbindung**: Verbindet sich automatisch mit "work"
- **API-Abfrage**: Ruft alle 60 Sekunden neue Texte ab
- **Display-Steuerung**: Zeigt Texte auf E-Ink Display an
- **Mehrzeilige Unterstützung**: Umbrechen langer Texte

### Datenbank
- **Tabelle**: `texts`
  - `id` (SERIAL PRIMARY KEY)
  - `content` (TEXT)
  - `created_at` (TIMESTAMP)

## Datenfluss

```
PostgreSQL-Datenbank (diglas.ddns.net:8564)
         ↓
Node.js Backend (localhost:3000)
         ↓
ESP32 (via HTTP)
         ↓
E-Ink Display (Waveshare 7.5")
```

## Ports

- **3000**: Backend-API (HTTP)
- **8564**: PostgreSQL-Datenbank

## Erforderliche Bibliotheken (ESP32)

### Standard (im ESP32 Core enthalten)
- WiFi.h
- HTTPClient.h

### Externe Bibliotheken
- ArduinoJson (v6.x)
- Waveshare E-Paper Library
  - DEV_Config.h
  - EPD.h
  - GUI_Paint.h

## Konfigurierbare Parameter

### ESP32 Code
```cpp
const char* ssid = "work";                  // WLAN SSID
const char* password = "letmeinplz!";       // WLAN Passwort
const char* apiUrl = "http://...";          // Backend URL
const unsigned long updateInterval = 60000; // Update-Intervall (ms)
```

### Backend
```
DB_HOST=diglas.ddns.net
DB_PORT=8564
DB_NAME=appdb
DB_USER=User198738
DB_PASSWORD=***
PORT=3000
```

