# 🆕 NEU-Symbol Feature

## ✨ Was ist neu?

Texte die zum ersten Mal auf dem Display angezeigt werden, erhalten für **2 Stunden** ein **[NEU]** Symbol!

## 🎯 Wie funktioniert es?

### 1. **Automatische Erkennung:**
- Wenn der ESP32 einen Text das erste Mal abruft, wird das Feld `first_shown` in der Datenbank gesetzt
- Die nächsten 2 Stunden wird dieser Text als "NEU" markiert
- Nach 2 Stunden verschwindet das [NEU] Symbol automatisch

### 2. **Darstellung auf dem Display:**
```
┌────────────────────────────────────┐
│                                    │
│  [NEU] Willkommen zum Event!      │  ← Neuer Text (< 2h)
│                                    │
│  [NEU] Wichtige Ankündigung        │  ← Neuer Text (< 2h)
│                                    │
│  Normaler Text ohne NEU            │  ← Alter Text (> 2h)
│                                    │
│  Vergiss nicht Wasser trinken      │  ← Alter Text
│                                    │
│                           10:15    │  ← Uhrzeit
└────────────────────────────────────┘
```

### 3. **Im Serial Monitor:**
```
Text 1 [NEU] (WICHTIG): Willkommen zum Event!
Text 2 [NEU]: Wichtige Ankündigung
Text 3: Normaler Text ohne NEU
```

## 🔄 Zeitablauf:

```
00:00 Uhr - Text erstellt
↓
10:00 Uhr - ESP32 ruft Text ab
            → first_shown wird gesetzt
            → [NEU] erscheint auf dem Display
↓
10:30 Uhr - [NEU] wird noch angezeigt (< 2h)
↓
11:00 Uhr - [NEU] wird noch angezeigt (< 2h)
↓
11:30 Uhr - [NEU] wird noch angezeigt (< 2h)
↓
12:00 Uhr - [NEU] wird noch angezeigt (< 2h)
↓
12:01 Uhr - [NEU] verschwindet (> 2h seit first_shown)
            → Text wird normal angezeigt
```

## 💾 Datenbank:

### Neues Feld: `first_shown`
- **Typ:** TIMESTAMP
- **Wert:** NULL = noch nie angezeigt, Datum/Zeit = erste Anzeige
- **Automatisch:** Wird beim ersten Abruf durch ESP32 gesetzt

### Beispiel:
```sql
SELECT id, content, first_shown, created_at 
FROM texts 
WHERE active = true;

 id |        content         |      first_shown        |      created_at
----+------------------------+-------------------------+-------------------------
  1 | Normaler Text          | 2025-10-23 08:00:00    | 2025-10-23 07:55:00
  2 | Neuer Text             | 2025-10-23 10:00:00    | 2025-10-23 09:58:00  ← NEU!
  3 | Noch nicht angezeigt   | NULL                   | 2025-10-23 10:05:00  ← Noch nie gezeigt
```

## 🎨 Visuelle Verbesserungen im Kalender:

### Kalender-Tage mit Texten sind jetzt DEUTLICH sichtbar:

#### **Vorher:**
- Leichter blauer Hintergrund
- Dünner Rand
- Schwer zu erkennen

#### **Jetzt:**
```
┌─────────────┐
│     15      │  ← Normale Tage
└─────────────┘

┏━━━━━━━━━━━━━┓
┃ ⭐   15    3┃  ← Tag mit Texten
┃ Geburtstag..┃     - Fetter Rand (3px)
┗━━━━━━━━━━━━━┛     - Gradient-Hintergrund
                     - ⭐ = Wichtiger Text
                     - 3 = Anzahl Texte (ROT)
                     - Textvorschau
```

### Neue Kalender-Features:
- ✅ **Fetter Rand** (3px statt 2px) für Tage mit Texten
- ✅ **Gradient-Hintergrund** (Blau-Verlauf)
- ✅ **Rote Zahl** zeigt Anzahl der Texte (größer + Schatten)
- ✅ **⭐ Symbol** für wichtige Texte (links oben)
- ✅ **Fette Schrift** für Tageszahl
- ✅ **Blaue Textvorschau** (25 Zeichen)

## 📊 API Response (für ESP32):

### Vorher:
```json
{
  "texts": [
    {
      "content": "Mein Text",
      "priority": false
    }
  ]
}
```

### Jetzt:
```json
{
  "texts": [
    {
      "content": "Mein Text",
      "priority": false,
      "isNew": true  ← NEU!
    }
  ]
}
```

## 🔧 Backend-Logik:

```javascript
// Prüfe ob Text neu ist (< 2 Stunden seit first_shown)
const firstShown = new Date(row.first_shown);
const hoursSinceFirstShown = (now - firstShown) / (1000 * 60 * 60);
const isNew = hoursSinceFirstShown < 2;

// Setze first_shown beim ersten Mal
if (!row.first_shown) {
  await pool.query(
    'UPDATE texts SET first_shown = $1 WHERE id = $2',
    [now, row.id]
  );
}
```

## 🎯 Anwendungsbeispiele:

### 1. **Breaking News:**
```
[NEU] EILMELDUNG: Wichtige Durchsage!
```
→ Fällt sofort auf, verschwindet nach 2 Stunden

### 2. **Neue Termine:**
```
[NEU] Morgen 15:00 Uhr: Team Meeting
```
→ Zeigt an dass der Termin gerade erst hinzugefügt wurde

### 3. **Aktuelle Ankündigungen:**
```
[NEU] Heute Nachmittag: Pizza Party!
```
→ Alle sehen dass es eine neue Ankündigung ist

## ⚙️ Technische Details:

### ESP32 Code:
```cpp
// Neues Array für isNew Flag
bool isNew[7];

// API gibt jetzt isNew zurück
isNew[count] = value["isNew"].as<bool>();

// [NEU] Präfix hinzufügen
if (isNew[i]) {
  displayText = "[NEU] " + displayText;
}
```

### Vorteile:
- ✅ Automatisch - keine manuelle Verwaltung
- ✅ Zeitbasiert - genau 2 Stunden
- ✅ Keine zusätzliche Konfiguration nötig
- ✅ Funktioniert mit allen Texten (normal + Kalender)
- ✅ Visuell deutlich erkennbar

## 🚀 Quick Start:

1. **Neuen Text erstellen** (Web-Interface oder Kalender)
2. **Warten** bis ESP32 das nächste Update macht (max. 60 Sek)
3. **[NEU]** erscheint vor dem Text
4. **Nach 2 Stunden** verschwindet [NEU] automatisch

## 📝 Status-Übersicht:

| Zeit seit first_shown | Status | Anzeige auf Display |
|----------------------|--------|---------------------|
| 0-2 Stunden          | NEU    | [NEU] Text...      |
| > 2 Stunden          | Normal | Text...            |
| Nie angezeigt        | -      | Wird nicht angezeigt (inaktiv) |

## 🎨 Komplette Darstellung:

```
Display um 10:05 Uhr:

┌────────────────────────────────────┐
│                                    │
│  [NEU] Heute Event um 15 Uhr!     │  ← Neu + Wichtig (Font24)
│                                    │
│  [NEU] Neue Mitteilung             │  ← Neu (Font20)
│                                    │
│  Reminder: Wasser trinken          │  ← Alt (Font20)
│                                    │
│  Status: Alles OK                  │  ← Alt (Font20)
│                                    │
│                           10:05    │  ← Uhrzeit
└────────────────────────────────────┘
```

## ✅ Vorteile:

1. **Aufmerksamkeit:** Neue Infos fallen sofort auf
2. **Automatisch:** Kein manuelles Entfernen nötig
3. **Zeitlimit:** Nach 2h ist die Info nicht mehr "neu"
4. **Konsistent:** Funktioniert für alle Texttypen
5. **Sichtbar:** Deutlich im Kalender + auf Display

---

**Alle Features sind fertig und funktionieren! 🎉**

