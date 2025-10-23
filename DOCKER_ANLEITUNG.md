# Docker Build und Start

## Build Image
```bash
docker build -t eink-display-backend .
```

## Start mit docker-compose
```bash
# 1. Erstelle .env Datei aus docker.env.example
cp docker.env.example .env

# 2. Bearbeite .env mit deinen Einstellungen (falls nötig)

# 3. Starte Container
docker-compose up -d

# 4. Logs anzeigen
docker-compose logs -f

# 5. Container stoppen
docker-compose down
```

## Manueller Start
```bash
# Build
docker build -t eink-display-backend .

# Run
docker run -d \
  --name eink-display-backend \
  -p 3000:3000 \
  -e DB_HOST=diglas.ddns.net \
  -e DB_PORT=8564 \
  -e DB_NAME=appdb \
  -e DB_USER=User198738 \
  -e DB_PASSWORD=sadfjkhk908ewrsdkfhjk \
  eink-display-backend

# Logs
docker logs -f eink-display-backend
```

## Port ändern
In docker-compose.yml die Zeile ändern:
```yaml
ports:
  - "DEIN_PORT:3000"  # z.B. "8080:3000"
```

Die interne Port 3000 bleibt gleich, nur der externe Port ändert sich.

