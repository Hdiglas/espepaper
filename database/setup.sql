-- PostgreSQL Datenbank Setup für E-Ink Display
-- 
-- Verbindung zur Datenbank herstellen:
-- psql -h diglas.ddns.net -p 8564 -U User198738 -d appdb

-- Tabelle für Texte erstellen
CREATE TABLE IF NOT EXISTS texts (
    id SERIAL PRIMARY KEY,
    content TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Index für schnellere Abfragen
CREATE INDEX IF NOT EXISTS idx_texts_created_at ON texts(created_at DESC);

-- Beispiel-Texte einfügen
INSERT INTO texts (content) VALUES 
    ('Hallo das ist Ein Test'),
    ('Willkommen beim E-Ink Display System'),
    ('Diese Texte kommen aus der PostgreSQL Datenbank');

-- Alle Texte anzeigen
SELECT * FROM texts ORDER BY created_at DESC;

