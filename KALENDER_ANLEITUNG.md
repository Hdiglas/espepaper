# 📅 Kalender-Feature Anleitung

## 🎉 Neues Feature: Kalender mit priorisierten Texten!

Das E-Ink Display Manager hat jetzt einen **Kalender-Tab**, mit dem du Texte für bestimmte Tage anlegen kannst!

## ✨ Hauptfunktionen:

### 1. **Zwei Tabs im Web-Interface:**
- **📝 Texte:** Normale Texte (wie bisher)
- **📅 Kalender:** Monatsansicht mit Tages-Texten

### 2. **Kalender-Texte:**
- Erstelle Texte für **bestimmte Tage**
- Texte für **heute** werden automatisch vorrangig angezeigt
- Optional: Markiere Texte als **"Wichtig"** (⭐)

### 3. **Priorisierung auf dem Display:**
- **Kalender-Texte für heute:** Werden **ZUERST** angezeigt
- **Wichtige Texte:** Werden in **größerer/fetterer Schrift** (Font24) angezeigt
- **Normale Texte:** Werden in normaler Schrift (Font20) angezeigt

## 🚀 So verwendest du den Kalender:

### **Schritt 1: Öffne den Kalender-Tab**
```
http://localhost:3000
```
Klicke auf den Tab "📅 Kalender"

### **Schritt 2: Wähle einen Tag**
- Du siehst den aktuellen Monat
- Der **heutige Tag** ist gelb markiert
- Tage mit Texten sind **blau** markiert und zeigen die Anzahl
- Klicke auf einen beliebigen Tag

### **Schritt 3: Füge einen Text hinzu**
Ein Modal öffnet sich:
1. Gib deinen Text ein (max. 200 Zeichen)
2. ✅ Optional: "Als wichtig markieren" für fette Darstellung
3. Klicke "Text hinzufügen"

### **Schritt 4: Aktiviere/Deaktiviere Texte**
- Aktivierte Texte werden auf dem Display angezeigt
- Maximum: **7 Texte** gleichzeitig aktiv
- Kalender-Texte werden automatisch priorisiert

## 📊 Beispiel-Anwendungen:

### 1. **Geburtstage:**
```
13. November → "🎂 Geburtstag von Max!" (Als wichtig markieren)
```
→ Wird am 13. November fett und zuerst angezeigt

### 2. **Termine:**
```
25. Oktober → "📅 Arzttermin um 14:00 Uhr"
28. Oktober → "🎬 Kino mit Freunden"
```

### 3. **Erinnerungen:**
```
1. November → "🗑️ Müll rausbringen"
15. November → "💰 Miete überweisen"
```

### 4. **Feiertage:**
```
24. Dezember → "🎄 Heiligabend - Weihnachtsfeier!" (wichtig)
31. Dezember → "🎆 Silvester Party"
```

## 🎯 Priorisierungs-Logik:

Das Display zeigt max. 7 Texte in dieser Reihenfolge:

1. **Kalender-Texte für HEUTE** (automatisch fett)
2. **Wichtige Kalender-Texte** (frühere Tage, falls markiert)
3. **Normale Kalender-Texte** (frühere Tage)
4. **Normale aktive Texte**

## 🖥️ Darstellung auf dem E-Ink Display:

```
┌──────────────────────────────────┐
│                                  │
│  🎂 Geburtstag von Max!         │  ← FETT (Font24) - Kalender-Text
│                                  │
│  Arzttermin um 14:00 Uhr        │  ← FETT (Font24) - Wichtig
│                                  │
│  Heute ist ein schoener Tag     │  ← Normal (Font20)
│                                  │
│  Vergiss nicht Wasser trinken   │  ← Normal (Font20)
│                                  │
│                                  │
│                         09:45   │  ← Uhrzeit
└──────────────────────────────────┘
```

## 🎨 Kalender-Ansicht:

### **Navigation:**
- **← Vorheriger:** Gehe zum vorherigen Monat
- **Heute:** Springe zum aktuellen Monat
- **Nächster →:** Gehe zum nächsten Monat

### **Tages-Ansicht:**
- **Gelber Rahmen:** Heute
- **Blauer Rahmen:** Tag hat Texte
- **Zahl rechts oben:** Anzahl der Texte für diesen Tag
- **Vorschau:** Erste 30 Zeichen des ersten Texts

## ⚙️ Technische Details:

### **Datenbank:**
Neue Spalten in der `texts` Tabelle:
- `display_date` (DATE): Das Datum für Kalender-Texte (NULL = normale Texte)
- `is_priority` (BOOLEAN): Ob der Text als wichtig markiert ist

### **API-Endpunkte:**
```
GET  /api/text/latest           → Aktive Texte (priorisiert)
GET  /api/calendar/:year/:month → Texte für einen Monat
POST /api/text                  → Neuen Text (mit display_date)
```

### **ESP32 Code:**
- Empfängt Texte mit `priority` Flag
- Font24 für priorisierte Texte
- Font20 für normale Texte
- Automatische Sortierung im Backend

## 💡 Tipps:

1. **Plane im Voraus:** Erstelle Texte für die nächsten Wochen
2. **Wichtige Termine:** Nutze die "Wichtig"-Checkbox
3. **Übersichtlichkeit:** Max. 2-3 Kalender-Texte pro Tag
4. **Textlänge:** Kalender-Texte kurz halten (30-50 Zeichen)
5. **Automatische Anzeige:** Texte für heute werden automatisch angezeigt

## 🔄 Update-Verhalten:

- ESP32 aktualisiert **alle 60 Sekunden**
- Am **Mitternacht** werden automatisch die Texte für den neuen Tag angezeigt
- Alte Kalender-Texte (vergangene Tage) werden **nicht** mehr angezeigt
- Du kannst sie aber weiterhin im Kalender sehen und verwalten

## 📝 Best Practices:

### **Gute Kalender-Texte:**
```
✅ "Geburtstag Lisa - 15:00 Uhr" (35 Zeichen)
✅ "Zahnarzt Termin 10:30" (23 Zeichen)
✅ "Urlaub beginnt heute!" (22 Zeichen)
```

### **Schlechte Kalender-Texte:**
```
❌ "Heute ist der Geburtstag von meiner besten Freundin Lisa und wir feiern um 15 Uhr in der..." (zu lang!)
❌ "Termin" (zu kurz, keine Info)
```

## 🆘 Problembehebung:

### **Problem: Kalender-Text wird nicht angezeigt**
**Lösung:**
1. Prüfe ob der Text **aktiviert** ist (grüner Status)
2. Prüfe ob das Datum **heute oder früher** ist
3. Maximal 7 Texte können aktiv sein

### **Problem: Text wird nicht fett angezeigt**
**Lösung:**
1. Muss ein Kalender-Text für **heute** sein, ODER
2. Muss als **"Wichtig"** markiert sein
3. ESP32 Code muss neu hochgeladen werden

### **Problem: Alter Kalender-Text wird noch angezeigt**
**Lösung:**
- Deaktiviere oder lösche alte Texte im Kalender-Tab

## 🚀 Schnellstart-Beispiel:

1. **Server starten** (läuft bereits)
2. **Web-Interface öffnen:** http://localhost:3000
3. **Kalender-Tab öffnen**
4. **Auf heute klicken**
5. **Text eingeben:** "Heute ist ein besonderer Tag!"
6. **"Als wichtig markieren" ✅**
7. **"Text hinzufügen" klicken**
8. **ESP32 neu hochladen** (mit dem neuen Code)
9. **Warten:** Max. 60 Sekunden
10. **✅ Der Text erscheint FETT auf dem Display!**

---

**Viel Spaß mit dem neuen Kalender-Feature! 🎉**

