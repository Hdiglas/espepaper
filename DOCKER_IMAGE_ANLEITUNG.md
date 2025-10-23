# 🐳 Docker Image - Komplette Anleitung

## Was ist jetzt anders?

**Vorher:** Code musste separat kopiert werden  
**Jetzt:** Alles ist im Docker Image!

## ⚡ Einfache GUI-Anleitung

### 1. **Dateien auf NAS kopieren**

Du brauchst nur diese 3 Dateien:
- `Dockerfile` (~264 Bytes)
- `docker-compose.yml` (~505 Bytes)
- `.env` (aus docker.env.example)

**NICHT mehr nötig:** `backend/` Ordner kopieren!

### 2. **Im Container Manager**

#### Option A: Docker Compose (empfohlen!)
1. Container Manager öffnen
2. "Compose" oder "Stack" auswählen
3. `docker-compose.yml` hochladen
4. `.env` Datei angeben
5. **Start!**

#### Option B: Manuell
1. Image aus `Dockerfile` bauen
2. Container erstellen mit:
   - Port: `3000:3000`
   - Environment-Variablen aus `.env`
   - Automatischer Neustart: Ja

### 3. **Fertig!**

Web-UI: `http://DEINE_NAS_IP:3000`

## 📦 Was ist im Image?

Das Docker Image enthält:
- Node.js 18
- Alle Backend-Files (`server.js`, etc.)
- Alle Dependencies (`node_modules`)
- Web-UI (`public/index.html`)

**Volumen:** ~200-300 MB

## 🔄 Updates

Wenn du Code ändern willst:

### Option 1: Neue Version hochladen
```bash
docker-compose down
# Neues Image von GitHub
docker-compose up -d --build
```

### Option 2: Live Editing (für Entwicklung)
Kommentiere die `volumes:` Zeile in `docker-compose.yml` wieder ein:
```yaml
volumes:
  - ./backend:/app
```

## ✅ Vorteile

- ✅ Einfacher zu deployen
- ✅ Alles in einem Image
- ✅ Keine Ordnerstruktur nötig
- ✅ Portabler
- ✅ Einfacher zu teilen

## 🚀 Schnellstart

1. `Dockerfile` → NAS
2. `docker-compose.yml` → NAS
3. `.env` → NAS
4. Container starten
5. Fertig!

## 📊 Vergleich

| Methode | Dateien | Komplexität |
|---------|---------|-------------|
| **Alt** | Dockerfile + backend/ | Mehrere Ordner |
| **Neu** | Nur Dockerfile | Ein Image! |

Das ist jetzt viel einfacher! 🎉

