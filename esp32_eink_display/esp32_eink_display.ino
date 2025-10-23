/* ESP32 E-Ink Display mit PostgreSQL Anbindung
 * Waveshare 7.5" B/W V2
 * 
 * Verbindet sich mit WLAN und ruft Texte von Backend-API ab
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include <stdlib.h>
#include <string.h>

// WLAN-Konfiguration
const char* ssid = "Work";
const char* password = "letmeinplz!";

// Backend-API URLs
const char* apiUrl = "http://192.168.0.162:3000/api/text/latest";
const char* fullscreenUrl = "http://192.168.0.162:3000/api/fullscreen-text/latest";
const char* settingsUrl = "http://192.168.0.162:3000/api/settings";

// Update-Intervall in Millisekunden (60 Sekunden)
const unsigned long updateInterval = 60000;
unsigned long lastUpdate = 0;

// Vollbild-Timer Variablen
unsigned long fullscreenInterval = 300000;  // 5 Minuten Standard
unsigned long fullscreenDuration = 60000;   // 60 Sekunden Standard
unsigned long lastFullscreenCheck = 0;
unsigned long fullscreenStartTime = 0;
bool isShowingFullscreen = false;
String currentFullscreenText = "";

// NTP Server für Zeitabfrage
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;  // GMT+1 (Deutschland)
const int daylightOffset_sec = 3600;  // Sommerzeit

// Bildpuffer
UBYTE *imageBuffer = NULL;

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 E-Ink Display startet...");
  
  // WLAN verbinden
  connectWiFi();
  
  // Zeit konfigurieren
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Warte auf Zeitsynchronisation...");
  delay(2000);
  
  // E-Ink Display initialisieren
  initDisplay();
  
  // Settings laden
  loadSettings();
  
  // Ersten Text abrufen und anzeigen
  Serial.println("\n=== ERSTE AKTUALISIERUNG ===");
  updateDisplay();
  lastUpdate = millis();
  Serial.println("=== SETUP ABGESCHLOSSEN ===");
  Serial.println("ESP32 wird jetzt alle 60 Sekunden automatisch aktualisieren...\n");
}

void loop() {
  // Prüfen, ob WLAN noch verbunden ist
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WLAN-Verbindung verloren. Neuverbindung...");
    connectWiFi();
  }
  
  // Berechne verbleibende Zeit bis zum nächsten Update
  unsigned long timeSinceLastUpdate = millis() - lastUpdate;
  unsigned long timeUntilNextUpdate = updateInterval - timeSinceLastUpdate;
  
  // Alle 10 Sekunden Countdown anzeigen
  static unsigned long lastCountdown = 0;
  if (millis() - lastCountdown >= 10000) {
    int secondsRemaining = timeUntilNextUpdate / 1000;
    Serial.print("Naechstes Update in ");
    Serial.print(secondsRemaining);
    Serial.println(" Sekunden...");
    lastCountdown = millis();
  }
  
  // Vollbild-Timer prüfen
  checkFullscreenTimer();
  
  // Prüfen, ob Update fällig ist
  if (timeSinceLastUpdate >= updateInterval) {
    Serial.println("\n=== UPDATE WIRD DURCHGEFUEHRT ===");
    updateDisplay();
    lastUpdate = millis();
    Serial.println("=== UPDATE ABGESCHLOSSEN ===\n");
  }
  
  delay(1000); // 1 Sekunde warten
}

void checkFullscreenTimer() {
  unsigned long now = millis();
  
  if (isShowingFullscreen) {
    // Prüfe ob Vollbild-Dauer abgelaufen ist
    if (now - fullscreenStartTime >= fullscreenDuration) {
      Serial.println(">>> Vollbild-Dauer abgelaufen, wechsle zur normalen Ansicht");
      isShowingFullscreen = false;
      updateDisplay(); // Zeige normale Texte
    }
  } else {
    // Prüfe ob Interval abgelaufen ist
    if (now - lastFullscreenCheck >= fullscreenInterval) {
      Serial.println(">>> Vollbild-Interval abgelaufen, pruefe Vollbild-Text");
      showFullscreenText();
      lastFullscreenCheck = now;
    }
  }
}

void showFullscreenText() {
  String text = fetchFullscreenText();
  if (text.length() > 0) {
    Serial.println(">>> Vollbild-Text gefunden, zeige an");
    isShowingFullscreen = true;
    fullscreenStartTime = millis();
    displayFullscreenText(text);
  } else {
    Serial.println(">>> Kein Vollbild-Text gefunden");
  }
}

String fetchFullscreenText() {
  if (WiFi.status() != WL_CONNECTED) {
    return "";
  }
  
  HTTPClient http;
  http.setTimeout(5000);
  http.begin(fullscreenUrl);
  
  int httpCode = http.GET();
  String text = "";
  
  if (httpCode == 200) {
    String payload = http.getString();
    
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, payload);
    
    if (!error && doc["success"] == true) {
      text = doc["text"].as<String>();
      if (text == "null" || text.length() == 0) {
        text = "";
      }
    }
  }
  
  http.end();
  return text;
}

void displayFullscreenText(String text) {
  Serial.println(">>> Aktualisiere Display mit Vollbild-Text...");
  
  // Display aufwecken
  EPD_7IN5_V2_Init();
  
  // Bildpuffer löschen
  Paint_Clear(WHITE);
  
  // Maximale Schriftgröße verwenden (Font24)
  int fontSize = Font24.Height;
  int textLen = text.length();
  int maxWidth = EPD_7IN5_V2_WIDTH - 40;
  int maxCharsPerLine = maxWidth / Font24.Width;
  
  // Berechne Anzahl Zeilen
  int numLines = (textLen / maxCharsPerLine) + 1;
  int totalHeight = numLines * (fontSize + 10);
  
  // Zentriere Text vertikal
  int startY = (EPD_7IN5_V2_HEIGHT - totalHeight) / 2;
  if (startY < 30) startY = 30; // Mindestabstand oben
  
  // Zeichne Text zeilenweise
  int pos = 0;
  int y = startY;
  
  while (pos < textLen && y < EPD_7IN5_V2_HEIGHT - 50) {
    int endPos = pos + maxCharsPerLine;
    if (endPos > textLen) {
      endPos = textLen;
    } else {
      // Versuche an Leerzeichen zu trennen
      int lastSpace = text.lastIndexOf(' ', endPos);
      if (lastSpace > pos) {
        endPos = lastSpace;
      }
    }
    
    String line = text.substring(pos, endPos);
    line.trim();
    
    if (line.length() > 0) {
      int lineWidth = line.length() * Font24.Width;
      int x = (EPD_7IN5_V2_WIDTH - lineWidth) / 2; // Zentriere horizontal
      
      Paint_DrawString_EN(x, y, line.c_str(), &Font24, WHITE, BLACK);
      y += fontSize + 10;
    }
    
    pos = endPos + 1;
  }
  
  // Display aktualisieren
  Serial.println(">>> Sende Vollbild-Text an Display...");
  EPD_7IN5_V2_Display(imageBuffer);
  
  // Warte kurz
  delay(1000);
  
  // Display in Sleep-Modus versetzen
  EPD_7IN5_V2_Sleep();
  
  Serial.println(">>> Vollbild-Text erfolgreich angezeigt!");
}

void loadSettings() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WARNUNG] WLAN nicht verbunden, verwende Standard-Settings");
    return;
  }
  
  HTTPClient http;
  http.setTimeout(5000);
  http.begin(settingsUrl);
  
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    String payload = http.getString();
    
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, payload);
    
    if (!error && doc["success"] == true) {
      if (doc["settings"]["fullscreen_interval"]) {
        fullscreenInterval = doc["settings"]["fullscreen_interval"].as<unsigned long>() * 1000;
        Serial.print("[OK] Vollbild-Interval geladen: ");
        Serial.print(fullscreenInterval / 1000);
        Serial.println(" Sekunden");
      }
      
      if (doc["settings"]["fullscreen_duration"]) {
        fullscreenDuration = doc["settings"]["fullscreen_duration"].as<unsigned long>() * 1000;
        Serial.print("[OK] Vollbild-Dauer geladen: ");
        Serial.print(fullscreenDuration / 1000);
        Serial.println(" Sekunden");
      }
    }
  } else {
    Serial.println("[WARNUNG] Konnte Settings nicht laden, verwende Standard-Werte");
  }
  
  http.end();
}

void connectWiFi() {
  Serial.print("Verbinde mit WLAN: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWLAN verbunden!");
    Serial.print("IP-Adresse: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWLAN-Verbindung fehlgeschlagen!");
  }
}

void initDisplay() {
  Serial.println("Initialisiere E-Ink Display...");
  
  DEV_Module_Init();
  EPD_7IN5_V2_Init();
  
  Serial.println("Loesche Display (erstes Mal)...");
  EPD_7IN5_V2_Clear();
  DEV_Delay_ms(500);
  
  // Bildpuffer allokieren
  UWORD imgSize = ((EPD_7IN5_V2_WIDTH % 8 == 0) ?
                   (EPD_7IN5_V2_WIDTH / 8) :
                   (EPD_7IN5_V2_WIDTH / 8 + 1)) * EPD_7IN5_V2_HEIGHT;
  
  if (imageBuffer == NULL) {
    imageBuffer = (UBYTE *)malloc(imgSize);
    if (!imageBuffer) {
      Serial.println("FEHLER: Speicher fuer Bildpuffer konnte nicht allokiert werden!");
      while (1) {
        delay(1000);
      }
    }
    Serial.print("Bildpuffer allokiert: ");
    Serial.print(imgSize);
    Serial.println(" Bytes");
  }
  
  Paint_NewImage(imageBuffer, EPD_7IN5_V2_WIDTH, EPD_7IN5_V2_HEIGHT, 0, WHITE);
  Paint_SelectImage(imageBuffer);
  
  Serial.println("Display initialisiert und bereit!");
}

void updateDisplay() {
  unsigned long startTime = millis();
  Serial.println("\n>>> Rufe Texte von API ab...");
  
  String texts[7];
  bool priorities[7];
  bool isNew[7];
  int count = fetchTextsFromAPI(texts, priorities, isNew);
  
  if (count > 0) {
    Serial.print(">>> Texte erhalten: ");
    Serial.println(count);
    displayMultipleTexts(texts, priorities, isNew, count);
  } else {
    Serial.println(">>> Keine Texte erhalten oder Fehler bei API-Abfrage");
    texts[0] = "Fehler beim Laden";
    priorities[0] = false;
    isNew[0] = false;
    displayMultipleTexts(texts, priorities, isNew, 1);
  }
  
  unsigned long duration = millis() - startTime;
  Serial.print(">>> Update dauerte ");
  Serial.print(duration);
  Serial.println(" ms");
}

// Struktur für Text mit Priorität
struct TextWithPriority {
  String content;
  bool priority;
};

int fetchTextsFromAPI(String texts[], bool priorities[], bool isNew[]) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WLAN nicht verbunden!");
    return 0;
  }
  
  Serial.print("Verbinde mit: ");
  Serial.println(apiUrl);
  
  HTTPClient http;
  http.setTimeout(10000); // 10 Sekunden Timeout
  http.begin(apiUrl);
  
  Serial.println("Sende HTTP GET Request...");
  int httpCode = http.GET();
  int count = 0;
  
  Serial.print("HTTP Status Code: ");
  Serial.println(httpCode);
  
  if (httpCode == 200) {
    String payload = http.getString();
    Serial.println("API-Antwort: " + payload);
    
    // JSON parsen mit größerem Buffer für mehrere Texte
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, payload);
    
    if (!error) {
      if (doc["success"] == true) {
        JsonArray textsArray = doc["texts"].as<JsonArray>();
        count = 0;
        for (JsonVariant value : textsArray) {
          if (count < 7) {
            if (value.is<JsonObject>()) {
              // Neues Format mit content, priority und isNew
              texts[count] = value["content"].as<String>();
              priorities[count] = value["priority"].as<bool>();
              isNew[count] = value["isNew"].as<bool>();
              Serial.print("Text ");
              Serial.print(count + 1);
              if (isNew[count]) Serial.print(" [NEU]");
              if (priorities[count]) Serial.print(" (WICHTIG)");
              Serial.print(": ");
              Serial.println(texts[count]);
            } else {
              // Altes Format (nur String)
              texts[count] = value.as<String>();
              priorities[count] = false;
              isNew[count] = false;
              Serial.print("Text ");
              Serial.print(count + 1);
              Serial.print(": ");
              Serial.println(texts[count]);
            }
            count++;
          }
        }
      } else {
        Serial.println("API meldet: success = false");
      }
    } else {
      Serial.print("JSON-Parse-Fehler: ");
      Serial.println(error.c_str());
    }
  } else if (httpCode < 0) {
    Serial.println("FEHLER: Keine Verbindung zum Server moeglich!");
    Serial.println("Pruefe:");
    Serial.println("1. Backend laeuft auf 192.168.0.162:3000");
    Serial.println("2. ESP32 ist im gleichen WLAN");
    Serial.println("3. Firewall erlaubt Verbindungen");
  } else {
    Serial.print("HTTP-Fehler: ");
    Serial.println(httpCode);
  }
  
  http.end();
  return count;
}

void displayMultipleTexts(String texts[], bool priorities[], bool isNew[], int count) {
  Serial.println(">>> Aktualisiere Display mit mehreren Texten...");
  
  // Display aufwecken falls im Sleep
  EPD_7IN5_V2_Init();
  
  // Bildpuffer löschen
  Paint_Clear(WHITE);
  
  // Texte anzeigen - mit Platz-Reserve für Uhrzeit
  int startY = 30;
  int lineSpacing = 15; // Mehr Abstand zwischen Texten
  int maxWidth = EPD_7IN5_V2_WIDTH - 40;
  int maxDisplayY = EPD_7IN5_V2_HEIGHT - 80; // Reserve für Uhrzeit
  
  Serial.print(">>> Zeige ");
  Serial.print(count);
  Serial.println(" Text(e) an:");
  
  for (int i = 0; i < count; i++) {
    // Prüfe ob noch Platz ist
    if (startY >= maxDisplayY) {
      Serial.print(">>> WARNUNG: Kein Platz mehr fuer Text ");
      Serial.println(i + 1);
      break;
    }
    
    // Begrenze Textlänge auf 200 Zeichen
    String displayText = texts[i];
    if (displayText.length() > 200) {
      displayText = displayText.substring(0, 197) + "...";
      Serial.print(">>> WARNUNG: Text ");
      Serial.print(i + 1);
      Serial.println(" wurde gekuerzt");
    }
    
    // Füge [NEU] Präfix hinzu wenn der Text neu ist
    if (isNew[i]) {
      displayText = "[NEU] " + displayText;
    }
    
    Serial.print("    ");
    Serial.print(i + 1);
    if (isNew[i]) Serial.print(". [NEU]");
    if (priorities[i]) Serial.print(" [WICHTIG]");
    Serial.print(" ");
    Serial.println(displayText);
    
    // Wähle Schriftgröße basierend auf Priorität
    // Wichtige Texte (Kalender-Texte) in Font24 (fett/größer)
    // Normale Texte in Font20
    sFONT* font = priorities[i] ? &Font24 : &Font20;
    
    int newY = drawWrappedText(displayText, 20, startY, maxWidth, maxDisplayY, font);
    
    // Prüfe ob Text gezeichnet werden konnte
    if (newY > startY) {
      startY = newY + lineSpacing;
    } else {
      Serial.println(">>> WARNUNG: Text konnte nicht gezeichnet werden");
      break;
    }
  }
  
  // Uhrzeit unten rechts anzeigen
  drawTime();
  
  // Display aktualisieren
  Serial.println(">>> Sende Daten an E-Ink Display...");
  Serial.println(">>> Bitte warten, E-Ink Display aktualisiert (ca. 15-20 Sekunden)...");
  
  EPD_7IN5_V2_Display(imageBuffer);
  
  Serial.println(">>> Warte auf Display-Bereitschaft...");
  delay(1000); // Kurze Pause
  
  // Display in Sleep-Modus versetzen (Strom sparen)
  EPD_7IN5_V2_Sleep();
  
  Serial.println(">>> Display erfolgreich aktualisiert und im Sleep-Modus!");
}

void drawTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println(">>> WARNUNG: Zeit konnte nicht abgerufen werden");
    return;
  }
  
  char timeStr[20];
  char dateStr[30];
  strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);
  strftime(dateStr, sizeof(dateStr), "%d.%m.%Y", &timeinfo);
  
  // Uhrzeit unten rechts positionieren
  int timeWidth = strlen(timeStr) * Font16.Width;
  int timeX = EPD_7IN5_V2_WIDTH - timeWidth - 20;
  int timeY = EPD_7IN5_V2_HEIGHT - Font16.Height - 20;
  
  Paint_DrawString_EN(timeX, timeY, timeStr, &Font16, WHITE, BLACK);
  
  Serial.print(">>> Aktuelle Uhrzeit: ");
  Serial.print(timeStr);
  Serial.print(" (");
  Serial.print(dateStr);
  Serial.println(")");
}

int drawWrappedText(String text, int x, int startY, int maxWidth, int maxY, sFONT* font) {
  int currentY = startY;
  int textLen = text.length();
  int startPos = 0;
  int maxCharsPerLine = maxWidth / font->Width;
  int linesDrawn = 0;
  int maxLines = 3; // Maximum 3 Zeilen pro Text
  
  // Sicherheitsprüfung
  if (startY < 0 || startY >= maxY) {
    Serial.println(">>> FEHLER: Ungueltige startY Position");
    return startY;
  }
  
  while (startPos < textLen && linesDrawn < maxLines) {
    int endPos = startPos + maxCharsPerLine;
    if (endPos > textLen) {
      endPos = textLen;
    } else {
      // Versuche an Leerzeichen zu trennen
      int lastSpace = text.lastIndexOf(' ', endPos);
      if (lastSpace > startPos && lastSpace < endPos) {
        endPos = lastSpace;
      }
    }
    
    String line = text.substring(startPos, endPos);
    line.trim();
    
    if (line.length() > 0) {
      // Prüfe ob noch Platz ist
      if (currentY + font->Height > maxY) {
        Serial.println(">>> Kein Platz mehr fuer weitere Zeilen");
        break;
      }
      
      // Begrenze die Zeilenlänge nochmal zur Sicherheit
      if (line.length() > 50) {
        line = line.substring(0, 47) + "...";
      }
      
      Paint_DrawString_EN(x, currentY, line.c_str(), font, WHITE, BLACK);
      currentY += font->Height + 3;
      linesDrawn++;
    }
    
    startPos = endPos + 1;
  }
  
  return currentY;
}


