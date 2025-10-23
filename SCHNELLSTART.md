# Schnellstart-Anleitung

## 🚀 In 3 Schritten zum Ziel

### Schritt 1: Backend starten

Öffne PowerShell im Projekt-Ordner und führe aus:

```powershell
cd backend
.\start.bat
```

Das Backend verbindet sich automatisch mit der PostgreSQL-Datenbank und läuft auf Port 3000.

**Wichtig:** Notiere dir die IP-Adresse deines Rechners:
```powershell
ipconfig
```
Du brauchst die "IPv4-Adresse" (z.B. `192.168.1.100`)

---

### Schritt 2: ESP32-Code anpassen

1. Öffne `esp32_eink_display/esp32_eink_display.ino` in Arduino IDE

2. Ändere diese Zeile (nutze deine IP-Adresse aus Schritt 1):
   ```cpp
   const char* apiUrl = "http://192.168.1.100:3000/api/text/latest";
   ```

3. Speichern!

---

### Schritt 3: Code auf ESP32 hochladen

1. Verbinde ESP32 via USB
2. In Arduino IDE: **Sketch → Hochladen**
3. Öffne den Seriellen Monitor (Strg+Shift+M)
4. Beobachte die Ausgabe:
   - ✓ WLAN-Verbindung
   - ✓ Text-Abruf von API
   - ✓ Display-Update

---

## ✅ Fertig!

Der ESP32 sollte jetzt:
- Mit dem WLAN "work" verbunden sein
- Alle 60 Sekunden neue Texte von der Datenbank abrufen
- Diese auf dem E-Ink Display anzeigen

---

## 🔧 Benötigte Software

Falls noch nicht installiert:

1. **Node.js**: https://nodejs.org (LTS Version)
2. **Arduino IDE**: https://www.arduino.cc/en/software
3. **ArduinoJson Bibliothek**: In Arduino IDE → Sketch → Bibliothek einbinden → Bibliotheken verwalten → "ArduinoJson" suchen
4. **Waveshare E-Paper Bibliothek**: https://github.com/waveshare/e-Paper

---

## 📝 Texte in der Datenbank verwalten

### Neuen Text hinzufügen (via API)

```powershell
Invoke-RestMethod -Uri "http://localhost:3000/api/text" -Method POST -ContentType "application/json" -Body '{"content":"Mein neuer Text"}'
```

### Alle Texte anzeigen

```powershell
Invoke-RestMethod -Uri "http://localhost:3000/api/texts" -Method GET
```

### Direkt in PostgreSQL

Verbinde dich mit der Datenbank und führe aus:
```sql
INSERT INTO texts (content) VALUES ('Dein neuer Text hier');
```

---

## ❓ Probleme?

Siehe `INSTALLATION.md` für detaillierte Fehlerbehebung.

**Häufigste Fehler:**
- Backend läuft nicht → Prüfe ob Port 3000 frei ist
- ESP32 findet Backend nicht → IP-Adresse korrekt?
- Display zeigt nichts → Verkabelung prüfen

