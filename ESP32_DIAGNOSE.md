# ESP32 Diagnose und Fehlerbehebung

## ✅ Automatische Aktualisierung

Der ESP32 ist bereits so programmiert, dass er **automatisch alle 60 Sekunden** die Datenbank abfragt und das Display aktualisiert.

### Was passiert automatisch:

1. **Alle 60 Sekunden:**
   - ESP32 ruft die API ab: `http://192.168.0.162:3000/api/text/latest`
   - Holt alle aktiven Texte (max. 7)
   - Aktualisiert die Uhrzeit
   - Sendet alles an das E-Ink Display

2. **Alle 10 Sekunden:**
   - Countdown im Serial Monitor: "Naechstes Update in XX Sekunden..."

### 🔍 So überprüfst du, ob es funktioniert:

#### 1. Serial Monitor öffnen (Arduino IDE)
- Tools → Serial Monitor
- Baudrate: **115200**

#### 2. Was du sehen solltest:
```
ESP32 E-Ink Display startet...
Verbinde mit WLAN: Work
WLAN verbunden!
IP-Adresse: 192.168.x.x

=== ERSTE AKTUALISIERUNG ===
>>> Rufe Texte von API ab...
HTTP Status Code: 200
>>> Texte erhalten: 7
>>> Zeige 7 Text(e) an:
    1. Text eins
    2. Text zwei
    ...
>>> Aktuelle Uhrzeit: 14:25 (23.10.2025)
>>> Display erfolgreich aktualisiert!
=== SETUP ABGESCHLOSSEN ===
ESP32 wird jetzt alle 60 Sekunden automatisch aktualisieren...

Naechstes Update in 50 Sekunden...
Naechstes Update in 40 Sekunden...
Naechstes Update in 30 Sekunden...
...

=== UPDATE WIRD DURCHGEFUEHRT ===
>>> Rufe Texte von API ab...
...
=== UPDATE ABGESCHLOSSEN ===
```

### ❌ Häufige Probleme:

#### Problem 1: "WLAN nicht verbunden!"
**Lösung:** 
- SSID/Passwort in Zeile 17-18 prüfen
- Router ist eingeschaltet
- ESP32 ist in Reichweite

#### Problem 2: "HTTP Status Code: -1" oder "Keine Verbindung"
**Lösung:**
- Backend läuft: `.\check-server.ps1`
- IP-Adresse stimmt (Zeile 22): `192.168.0.162`
- Firewall erlaubt Port 3000
- ESP32 und PC im gleichen WLAN

#### Problem 3: "JSON-Parse-Fehler"
**Lösung:**
- Server neu starten
- API manuell testen: `Invoke-WebRequest http://localhost:3000/api/text/latest`

#### Problem 4: Display aktualisiert nicht
**Lösung:**
- Neuen Code hochladen (ist jetzt mit mehr Debug-Info)
- Reset-Button am ESP32 drücken
- Serial Monitor beobachten

### 🚀 Schnelltest:

1. **Server-Status prüfen:**
   ```powershell
   .\check-server.ps1
   ```

2. **ESP32 neu starten:**
   - Reset-Button drücken

3. **Serial Monitor öffnen:**
   - Baudrate 115200
   - Countdown beobachten

4. **Text im Web-Interface ändern:**
   - http://localhost:3000 öffnen
   - Text aktivieren/deaktivieren
   - Maximal 60 Sekunden warten

### 📊 Erwartetes Verhalten:

- **0 Sekunden:** ESP32 startet, erste Aktualisierung
- **60 Sekunden:** Automatische Aktualisierung (neue Texte + neue Uhrzeit)
- **120 Sekunden:** Automatische Aktualisierung (neue Texte + neue Uhrzeit)
- **180 Sekunden:** Automatische Aktualisierung (neue Texte + neue Uhrzeit)
- usw.

**Wichtig:** Das Display wird IMMER aktualisiert (auch wenn die Texte gleich sind), weil sich die Uhrzeit ändert!

### 🛠️ Wenn es immer noch nicht funktioniert:

1. **Vollständiger Neustart:**
   ```powershell
   # Server stoppen
   Get-Process node | Stop-Process -Force
   
   # Server neu starten
   cd "D:\Ugreen Privat\Programming and Ai\Teseinkscreen postgres for all\backend"
   node server.js
   ```

2. **ESP32 Code erneut hochladen:**
   - Arduino IDE öffnen
   - `esp32_eink_display.ino` öffnen
   - Upload (Strg+U)
   - Serial Monitor öffnen (Strg+Shift+M)

3. **IP-Adresse des Servers prüfen:**
   ```powershell
   ipconfig
   # Suche nach "IPv4-Adresse" im WLAN-Adapter
   # Muss mit der IP in Zeile 22 der .ino Datei übereinstimmen
   ```

### 💡 Tipps:

- E-Ink Displays sind langsam (ca. 15-20 Sekunden für vollständige Aktualisierung)
- Der Serial Monitor zeigt dir ALLES, was passiert
- Der Countdown hilft dir zu sehen, dass der ESP32 läuft
- Bei Änderungen im Web-Interface maximal 60 Sekunden warten

