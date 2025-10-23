FROM node:18-alpine

WORKDIR /app

# Kopiere package.json und installiere Dependencies
COPY backend/package*.json ./
RUN npm install

# Kopiere Backend Code
COPY backend/ .

# Exponiere Port
EXPOSE 3000

# Starte Server
CMD ["node", "server.js"]

