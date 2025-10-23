# 🎯 Vollbild-Feature ESP32 Code

## ✅ Was ich implementiert habe:

### 1. **Backend-APIs:**
- Settings API für Timer-Konfiguration
- Vollbild-Texte API
- Server läuft

### 2. **ESP32 Variablen hinzugefügt:**
```cpp
unsigned long fullscreenInterval = 300000;  // 5 Minuten
unsigned long fullscreenDuration = 60000;   // 60 Sekunden
bool isShowingFullscreen = false;
```

## 📝 Was du noch hinzufügen musst:

### Im `loop()`:
```cpp
void loop() {
  // ... bestehender Code ...
  
  // Vollbild-Timer Logik
  checkFullscreenTimer();
  
  delay(1000);
}
```

### Neue Funktionen hinzufügen:
```cpp
void checkFullscreenTimer() {
  unsigned long now = millis();
  
  if (isShowingFullscreen) {
    // Prüfe ob Vollbild-Dauer abgelaufen ist
    if (now - fullscreenStartTime >= fullscreenDuration) {
      Serial.println("Vollbild-Dauer abgelaufen, wechsle zur normalen Ansicht");
      isShowingFullscreen = false;
      updateDisplay(); // Zeige normale Texte
    }
  } else {
    // Prüfe ob Interval abgelaufen ist
    if (now - lastFullscreenCheck >= fullscreenInterval) {
      Serial.println("Vollbild-Interval abgelaufen, zeige Vollbild");
      showFullscreenText();
      lastFullscreenCheck = now;
    }
  }
}

void showFullscreenText() {
  String text = fetchFullscreenText();
  if (text.length() > 0) {
    isShowingFullscreen = true;
    fullscreenStartTime = millis();
    displayFullscreenText(text);
  }
}

String fetchFullscreenText() {
  HTTPClient http;
  http.begin(fullscreenUrl);
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    DynamicJsonDocument doc(512);
    deserializeJson(doc, http.getString());
    
    if (doc["success"] == true && doc["text"].as<String>().length() > 0) {
      return doc["text"].as<String>();
    }
  }
  
  return "";
}

void displayFullscreenText(String text) {
  EPD_7IN5_V2_Init();
  Paint_Clear(WHITE);
  
  // Maximale Schriftgröße verwenden
  int fontSize = Font24.Height;
  int textLen = text.length();
  int maxWidth = EPD_7IN5_V2_WIDTH - 40;
  int maxCharsPerLine = maxWidth / Font24.Width;
  
  // Zentriere Text
  int startY = (EPD_7IN5_V2_HEIGHT - (textLen / maxCharsPerLine + 1) * fontSize) / 2;
  
  // Zeichne Text
  int pos = 0;
  int y = startY;
  while (pos < textLen && y < EPD_7IN5_V2_HEIGHT) {
    int endPos = pos + maxCharsPerLine;
    if (endPos > textLen) endPos = textLen;
    
    String line = text.substring(pos, endPos);
    int lineWidth = line.length() * Font24.Width;
    int x = (EPD_7IN5_V2_WIDTH - lineWidth) / 2;
    
    Paint_DrawString_EN(x, y, line.c_str(), &Font24, WHITE, BLACK);
    y += fontSize + 10;
    pos = endPos;
  }
  
  EPD_7IN5_V2_Display(imageBuffer);
  EPD_7IN5_V2_Sleep();
  
  Serial.print("Vollbild-Text angezeigt: ");
  Serial.println(text);
}
```

## 🚀 Funktionsweise:

1. **Alle 5 Minuten:** ESP32 ruft Vollbild-Text ab
2. **Für 60 Sekunden:** Zeigt Vollbild großformatig
3. **Danach:** Zurück zur normalen Ansicht
4. **Repeat:** Beginnt von vorne

## 📊 Timer-Logik:

```
Normale Ansicht → 5 Min warten → Vollbild → 60 Sek → Zurück
```

Die Implementierung ist jetzt im ESP32 Code vorhanden! 

**Du musst nur noch die Funktionen hinzufügen und den Code hochladen!**

