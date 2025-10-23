# 🚀 START HIER

Willkommen zum E-Ink Display mit PostgreSQL Projekt!

## ⚡ Schnellstart (3 einfache Schritte)

### 1️⃣ Backend starten

Öffne PowerShell in diesem Ordner und führe aus:

```powershell
cd backend
.\start.bat
```

✅ **Du solltest sehen:**
```
✓ Erfolgreich mit PostgreSQL verbunden
✓ Tabelle "texts" ist bereit
✓ Beispiel-Text eingefügt (oder X Text(e) vorhanden)
Backend-Server läuft auf Port 3000
```

### 2️⃣ Deine IP-Adresse ermitteln

```powershell
ipconfig
```

Notiere dir die **IPv4-Adresse** (z.B. `192.168.1.100`)

### 3️⃣ ESP32 programmieren

1. Öffne `esp32_eink_display\esp32_eink_display.ino` in Arduino IDE

2. **Ändere diese Zeile** (nutze deine IP-Adresse):
   ```cpp
   const char* apiUrl = "http://192.168.1.100:3000/api/text/latest";
   ```
   ↑ Ersetze `192.168.1.100` mit deiner tatsächlichen IP-Adresse!

3. **Lade den Code hoch:**
   - ESP32 via USB verbinden
   - In Arduino IDE: **Sketch → Hochladen**

4. **Öffne den Seriellen Monitor:**
   - Werkzeuge → Serieller Monitor (115200 Baud)
   - Du solltest sehen:
     ```
     Verbinde mit WLAN: work
     WLAN verbunden!
     Rufe Text von API ab...
     Text erhalten: Hallo das ist Ein Test
     Display aktualisiert!
     ```

---

## 📚 Dokumentation

- **[SCHNELLSTART.md](SCHNELLSTART.md)** - Ausführliche Schnellstart-Anleitung
- **[README.md](README.md)** - Projekt-Übersicht
- **[INSTALLATION.md](INSTALLATION.md)** - Detaillierte Installation
- **[BEFEHLE.md](BEFEHLE.md)** - Nützliche Befehle
- **[FEHLERBEHEBUNG.md](FEHLERBEHEBUNG.md)** - Hilfe bei Problemen
- **[PROJEKTSTRUKTUR.md](PROJEKTSTRUKTUR.md)** - Projekt-Aufbau

---

## 🔧 Benötigte Software

Falls noch nicht vorhanden, installiere:

### Node.js
Download: https://nodejs.org (LTS Version)

### Arduino IDE
Download: https://www.arduino.cc/en/software

### Arduino-Bibliotheken

1. **ESP32 Board:**
   - Arduino IDE → Datei → Voreinstellungen
   - Zusätzliche Boardverwalter-URLs:
     ```
     https://dl.espressif.com/dl/package_esp32_index.json
     ```
   - Werkzeuge → Board → Boardverwalter → "ESP32" installieren

2. **ArduinoJson:**
   - Sketch → Bibliothek einbinden → Bibliotheken verwalten
   - "ArduinoJson" suchen und installieren

3. **Waveshare E-Paper:**
   - Download: https://github.com/waveshare/e-Paper
   - Ordner aus `Arduino/epd` in Arduino libraries kopieren

---

## 📝 Texte verwalten

### Neuen Text hinzufügen

**Via PowerShell-Script:**
```powershell
cd backend
.\add-text.ps1 "Mein neuer toller Text"
```

**Via direktem Befehl:**
```powershell
Invoke-RestMethod -Uri "http://localhost:3000/api/text" -Method POST -ContentType "application/json" -Body '{"content":"Mein Text"}'
```

### Alle Texte anzeigen
```powershell
Invoke-RestMethod -Uri "http://localhost:3000/api/texts"
```

---

## ✅ Funktioniert es?

### Backend testen
```powershell
curl http://localhost:3000/health
```
Sollte `{"status":"OK"}` zurückgeben.

### Text-API testen
```powershell
Invoke-RestMethod -Uri "http://localhost:3000/api/text/latest"
```
Sollte `{"success":true,"text":"..."}` zurückgeben.

---

## ❓ Probleme?

1. **Backend startet nicht?**
   → Prüfe ob Node.js installiert ist: `node --version`

2. **ESP32 verbindet sich nicht mit WLAN?**
   → Stelle sicher, dass es ein 2.4 GHz WLAN ist (ESP32 unterstützt kein 5 GHz)

3. **Display zeigt nichts?**
   → Prüfe Verkabelung und Bibliotheken

4. **Weitere Hilfe?**
   → Siehe [FEHLERBEHEBUNG.md](FEHLERBEHEBUNG.md)

---

## 🎯 Konfiguration

### WLAN
- **SSID:** work
- **Passwort:** letmeinplz!

### PostgreSQL
- **Host:** diglas.ddns.net
- **Port:** 8564
- **Datenbank:** appdb
- **Benutzer:** User198738

### Backend
- **Port:** 3000
- **Konfiguration:** `backend/config.env`

---

## 🔄 Wie es funktioniert

```
PostgreSQL (diglas.ddns.net:8564)
          ↓
  Backend API (localhost:3000)
          ↓
      ESP32 (via WLAN)
          ↓
  E-Ink Display (zeigt Text an)
```

Der ESP32 ruft alle **60 Sekunden** einen neuen Text aus der Datenbank ab und zeigt ihn auf dem Display an.

---

## 💡 Tipp

Lasse das Backend immer laufen, während der ESP32 in Betrieb ist!

**Viel Erfolg! 🎉**

