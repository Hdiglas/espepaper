# Installations-Anleitung

## Schritt-für-Schritt Installation

### 1. Backend-API einrichten

#### Node.js installieren (falls noch nicht vorhanden)
Lade Node.js von https://nodejs.org herunter und installiere es.

#### Backend starten

```powershell
cd backend
npm install
npm start
```

Das Backend sollte jetzt laufen und folgende Ausgabe zeigen:
```
✓ Erfolgreich mit PostgreSQL verbunden
Backend-Server läuft auf Port 3000
Datenbank: diglas.ddns.net:8564/appdb
```

### 2. Datenbank einrichten (optional)

Falls die Tabelle `texts` noch nicht existiert, führe das SQL-Skript aus:

```powershell
psql -h diglas.ddns.net -p 8564 -U User198738 -d appdb -f database/setup.sql
```

Oder manuell über einen PostgreSQL-Client deiner Wahl.

### 3. ESP32 programmieren

#### Arduino IDE einrichten

1. Installiere Arduino IDE (https://www.arduino.cc/en/software)
2. Füge ESP32 Board hinzu:
   - Datei → Voreinstellungen
   - Bei "Zusätzliche Boardverwalter-URLs" einfügen:
     ```
     https://dl.espressif.com/dl/package_esp32_index.json
     ```
   - Werkzeuge → Board → Boardverwalter → "ESP32" suchen und installieren

#### Bibliotheken installieren

1. ArduinoJson:
   - Sketch → Bibliothek einbinden → Bibliotheken verwalten
   - "ArduinoJson" suchen und installieren (Version 6.x)

2. Waveshare E-Paper Bibliothek:
   - Lade die Bibliothek von https://github.com/waveshare/e-Paper herunter
   - Kopiere die Ordner aus `Arduino/epd` in dein Arduino libraries Verzeichnis

#### Backend-IP-Adresse konfigurieren

1. Finde die IP-Adresse deines Rechners:
   ```powershell
   ipconfig
   ```
   Suche nach "IPv4-Adresse" (z.B. 192.168.1.100)

2. Öffne `esp32_eink_display/esp32_eink_display.ino`
3. Ändere diese Zeile:
   ```cpp
   const char* apiUrl = "http://192.168.1.100:3000/api/text/latest";
   ```
   Ersetze `192.168.1.100` durch deine tatsächliche IP-Adresse

#### Code hochladen

1. Verbinde den ESP32 via USB
2. Werkzeuge → Board → ESP32 Arduino → "ESP32 Dev Module" (oder dein spezifisches Board)
3. Werkzeuge → Port → Wähle den COM-Port des ESP32
4. Klicke auf "Hochladen"

### 4. Testen

1. Öffne den Seriellen Monitor (Werkzeuge → Serieller Monitor, 115200 Baud)
2. Der ESP32 sollte sich mit WLAN verbinden
3. Texte von der API abrufen
4. Das Display sollte den Text anzeigen

### Fehlerbehebung

#### ESP32 verbindet sich nicht mit WLAN
- Prüfe SSID und Passwort
- Stelle sicher, dass das WLAN 2.4 GHz ist (ESP32 unterstützt kein 5 GHz)

#### API-Verbindung schlägt fehl
- Prüfe, ob das Backend läuft
- Prüfe die IP-Adresse im ESP32-Code
- Stelle sicher, dass ESP32 und Backend im selben Netzwerk sind
- Prüfe Firewall-Einstellungen

#### Display zeigt nichts an
- Prüfe die Verkabelung zwischen ESP32 und E-Ink Display
- Stelle sicher, dass die Waveshare-Bibliothek korrekt installiert ist

#### Datenbank-Verbindung schlägt fehl
- Prüfe die Zugangsdaten
- Stelle sicher, dass der PostgreSQL-Server erreichbar ist
- Prüfe Port 8564 (ist er geöffnet?)

