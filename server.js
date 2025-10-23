const express = require('express');
const { Pool } = require('pg');
const cors = require('cors');
const path = require('path');
require('dotenv').config();

const app = express();
const port = process.env.PORT || 3000;

// Middleware
app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));

// PostgreSQL Pool
const pool = new Pool({
  host: process.env.DB_HOST,
  port: process.env.DB_PORT,
  database: process.env.DB_NAME,
  user: process.env.DB_USER,
  password: process.env.DB_PASSWORD,
  ssl: false // Auf true setzen, falls SSL erforderlich
});

// Test der Datenbankverbindung und Tabelle erstellen
async function initDatabase() {
  try {
    const client = await pool.connect();
    console.log('✓ Erfolgreich mit PostgreSQL verbunden');
    
    // Tabelle erstellen falls nicht vorhanden mit active Feld und Datum
    await client.query(`
      CREATE TABLE IF NOT EXISTS texts (
        id SERIAL PRIMARY KEY,
        content TEXT NOT NULL,
        active BOOLEAN DEFAULT false,
        display_date DATE,
        is_priority BOOLEAN DEFAULT false,
        first_shown TIMESTAMP,
        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
      )
    `);
    
    // Spalte active hinzufügen falls sie noch nicht existiert (Migration)
    await client.query(`
      DO $$ 
      BEGIN 
        IF NOT EXISTS (
          SELECT 1 FROM information_schema.columns 
          WHERE table_name='texts' AND column_name='active'
        ) THEN
          ALTER TABLE texts ADD COLUMN active BOOLEAN DEFAULT false;
        END IF;
        
        IF NOT EXISTS (
          SELECT 1 FROM information_schema.columns 
          WHERE table_name='texts' AND column_name='display_date'
        ) THEN
          ALTER TABLE texts ADD COLUMN display_date DATE;
        END IF;
        
        IF NOT EXISTS (
          SELECT 1 FROM information_schema.columns 
          WHERE table_name='texts' AND column_name='is_priority'
        ) THEN
          ALTER TABLE texts ADD COLUMN is_priority BOOLEAN DEFAULT false;
        END IF;
        
        IF NOT EXISTS (
          SELECT 1 FROM information_schema.columns 
          WHERE table_name='texts' AND column_name='first_shown'
        ) THEN
          ALTER TABLE texts ADD COLUMN first_shown TIMESTAMP;
        END IF;
      END $$;
    `);
    
    console.log('[OK] Tabelle "texts" ist bereit');
    
    // Prüfen ob Texte vorhanden sind
    const result = await client.query('SELECT COUNT(*) FROM texts');
    const count = parseInt(result.rows[0].count);
    
    if (count === 0) {
      // Beispiel-Texte einfügen
      await client.query(
        'INSERT INTO texts (content, active) VALUES ($1, $2)',
        ['Willkommen beim E-Ink Display Manager!', true]
      );
      console.log('[OK] Beispiel-Text eingefuegt');
    } else {
      console.log(`[OK] ${count} Text(e) in der Datenbank vorhanden`);
    }
    
    client.release();
  } catch (err) {
    console.error('[FEHLER] bei der Datenbankinitialisierung:', err.stack);
  }
}

// Settings Tabelle erstellen
async function initSettings() {
  try {
    const client = await pool.connect();
    
    await client.query(`
      CREATE TABLE IF NOT EXISTS settings (
        id SERIAL PRIMARY KEY,
        key TEXT UNIQUE NOT NULL,
        value TEXT NOT NULL,
        updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
      )
    `);
    
    // Standard-Einstellungen einfügen
    await client.query(`
      INSERT INTO settings (key, value) 
      VALUES ('fullscreen_interval', '300'), ('fullscreen_duration', '60')
      ON CONFLICT (key) DO NOTHING
    `);
    
    console.log('[OK] Settings-Tabelle bereit');
    client.release();
  } catch (err) {
    console.error('[FEHLER] bei Settings-Initialisierung:', err.stack);
  }
}

// Vollbild-Texte Tabelle erstellen
async function initFullscreenTexts() {
  try {
    const client = await pool.connect();
    
    await client.query(`
      CREATE TABLE IF NOT EXISTS fullscreen_texts (
        id SERIAL PRIMARY KEY,
        content TEXT NOT NULL,
        active BOOLEAN DEFAULT false,
        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
      )
    `);
    
    console.log('[OK] Fullscreen-Texte Tabelle bereit');
    client.release();
  } catch (err) {
    console.error('[FEHLER] bei Fullscreen-Texte-Initialisierung:', err.stack);
  }
}

// Route: Aktive Texte für Display abrufen (max 7, priorisiert nach Datum)
app.get('/api/text/latest', async (req, res) => {
  try {
    const today = new Date().toISOString().split('T')[0];
    const now = new Date();
    
    // Hole Texte: Zuerst heutige Kalender-Texte, dann normale aktive Texte
    const result = await pool.query(`
      SELECT id, content, is_priority, display_date, first_shown, created_at
      FROM texts 
      WHERE active = true 
        AND (display_date IS NULL OR display_date <= $1)
      ORDER BY 
        CASE WHEN display_date = $1 THEN 0 ELSE 1 END,
        is_priority DESC,
        created_at DESC
      LIMIT 7
    `, [today]);
    
    if (result.rows.length > 0) {
      // Setze first_shown für Texte die noch nie angezeigt wurden
      for (const row of result.rows) {
        if (!row.first_shown) {
          await pool.query(
            'UPDATE texts SET first_shown = $1 WHERE id = $2',
            [now, row.id]
          );
          row.first_shown = now;
          console.log(`[INFO] Text ${row.id} zum ersten Mal angezeigt`);
        }
      }
      
      const texts = result.rows.map(row => {
        // Prüfe ob Text neu ist (< 2 Stunden seit first_shown)
        const firstShown = new Date(row.first_shown);
        const hoursSinceFirstShown = (now - firstShown) / (1000 * 60 * 60);
        const isNew = hoursSinceFirstShown < 2;
        
        return {
          content: row.content,
          priority: row.is_priority || (row.display_date === today),
          isNew: isNew
        };
      });
      
      res.json({ 
        success: true,
        texts: texts,
        count: texts.length
      });
    } else {
      res.json({ 
        success: true,
        texts: [{ content: 'Keine aktiven Texte', priority: false, isNew: false }],
        count: 0
      });
    }
  } catch (error) {
    console.error('[FEHLER] beim Abrufen der Texte:', error);
    res.status(500).json({ 
      success: false,
      error: 'Datenbankfehler' 
    });
  }
});

// Route: Alle Texte abrufen (für Web-UI)
app.get('/api/texts', async (req, res) => {
  try {
    const result = await pool.query(
      'SELECT id, content, active, display_date, is_priority, first_shown, created_at FROM texts ORDER BY created_at DESC'
    );
    
    res.json({ 
      success: true,
      texts: result.rows 
    });
  } catch (error) {
    console.error('[FEHLER] beim Abrufen der Texte:', error);
    res.status(500).json({ 
      success: false,
      error: 'Datenbankfehler' 
    });
  }
});

// Route: Kalender-Texte für einen bestimmten Monat abrufen
app.get('/api/calendar/:year/:month', async (req, res) => {
  try {
    const { year, month } = req.params;
    const monthNum = parseInt(month);
    
    // Berechne den ersten Tag des Monats
    const startDate = new Date(year, monthNum - 1, 1);
    
    // Berechne den letzten Tag des Monats (erster Tag des nächsten Monats - 1 Tag)
    const endDate = new Date(year, monthNum, 0);
    
    // Formatiere Datumsangaben für SQL
    const startDateStr = startDate.toISOString().split('T')[0];
    const endDateStr = endDate.toISOString().split('T')[0];
    
    const result = await pool.query(
      `SELECT id, content, display_date, active, is_priority 
       FROM texts 
       WHERE display_date >= $1 AND display_date <= $2
       ORDER BY display_date ASC`,
      [startDateStr, endDateStr]
    );
    
    res.json({
      success: true,
      texts: result.rows
    });
  } catch (error) {
    console.error('[FEHLER] beim Abrufen der Kalender-Texte:', error);
    res.status(500).json({
      success: false,
      error: 'Datenbankfehler'
    });
  }
});

// Route: Neuen Text hinzufügen
app.post('/api/text', async (req, res) => {
  let { content, display_date, is_priority } = req.body;
  
  if (!content) {
    return res.status(400).json({ 
      success: false,
      error: 'Text-Inhalt erforderlich' 
    });
  }
  
  // Begrenze Textlänge auf 200 Zeichen (E-Ink Display Limit)
  if (content.length > 200) {
    content = content.substring(0, 200);
    console.log('[INFO] Text wurde auf 200 Zeichen gekuerzt');
  }
  
  try {
    // Prüfen wie viele Texte bereits aktiv sind
    const countResult = await pool.query(
      'SELECT COUNT(*) FROM texts WHERE active = true'
    );
    const activeCount = parseInt(countResult.rows[0].count);
    
    // Automatisch aktivieren wenn weniger als 7 aktiv sind
    const shouldActivate = activeCount < 7;
    
    const result = await pool.query(
      'INSERT INTO texts (content, active, display_date, is_priority) VALUES ($1, $2, $3, $4) RETURNING id, content, active, display_date, is_priority, created_at',
      [content, shouldActivate, display_date || null, is_priority || false]
    );
    
    res.json({ 
      success: true,
      text: result.rows[0] 
    });
  } catch (error) {
    console.error('[FEHLER] beim Hinzufuegen des Texts:', error);
    res.status(500).json({ 
      success: false,
      error: 'Datenbankfehler' 
    });
  }
});

// Route: Text aktivieren/deaktivieren
app.put('/api/text/:id/toggle', async (req, res) => {
  const { id } = req.params;
  const { active } = req.body;
  
  try {
    // Wenn aktivieren, prüfen ob bereits 7 aktiv sind
    if (active) {
      const countResult = await pool.query(
        'SELECT COUNT(*) FROM texts WHERE active = true'
      );
      const activeCount = parseInt(countResult.rows[0].count);
      
      if (activeCount >= 7) {
        return res.status(400).json({
          success: false,
          error: 'Maximal 7 Texte können gleichzeitig aktiv sein'
        });
      }
    }
    
    const result = await pool.query(
      'UPDATE texts SET active = $1 WHERE id = $2 RETURNING id, content, active, display_date, is_priority, created_at',
      [active, id]
    );
    
    if (result.rows.length > 0) {
      res.json({
        success: true,
        text: result.rows[0]
      });
    } else {
      res.status(404).json({
        success: false,
        error: 'Text nicht gefunden'
      });
    }
  } catch (error) {
    console.error('[FEHLER] beim Aktualisieren:', error);
    res.status(500).json({
      success: false,
      error: 'Datenbankfehler'
    });
  }
});

// Route: Text löschen
app.delete('/api/text/:id', async (req, res) => {
  const { id } = req.params;
  
  try {
    const result = await pool.query(
      'DELETE FROM texts WHERE id = $1 RETURNING id',
      [id]
    );
    
    if (result.rows.length > 0) {
      res.json({
        success: true,
        message: 'Text geloescht'
      });
    } else {
      res.status(404).json({
        success: false,
        error: 'Text nicht gefunden'
      });
    }
  } catch (error) {
    console.error('[FEHLER] beim Loeschen:', error);
    res.status(500).json({
      success: false,
      error: 'Datenbankfehler'
    });
  }
});

// API: Settings abrufen
app.get('/api/settings', async (req, res) => {
  try {
    const result = await pool.query('SELECT key, value FROM settings');
    const settings = {};
    result.rows.forEach(row => {
      settings[row.key] = row.value;
    });
    res.json({ success: true, settings });
  } catch (error) {
    console.error('[FEHLER] beim Abrufen der Settings:', error);
    res.status(500).json({ success: false, error: 'Datenbankfehler' });
  }
});

// API: Settings aktualisieren
app.put('/api/settings', async (req, res) => {
  try {
    const { fullscreen_interval, fullscreen_duration } = req.body;
    
    if (fullscreen_interval) {
      await pool.query(
        'INSERT INTO settings (key, value) VALUES ($1, $2) ON CONFLICT (key) DO UPDATE SET value = $2',
        ['fullscreen_interval', fullscreen_interval]
      );
    }
    
    if (fullscreen_duration) {
      await pool.query(
        'INSERT INTO settings (key, value) VALUES ($1, $2) ON CONFLICT (key) DO UPDATE SET value = $2',
        ['fullscreen_duration', fullscreen_duration]
      );
    }
    
    res.json({ success: true, message: 'Settings aktualisiert' });
  } catch (error) {
    console.error('[FEHLER] beim Aktualisieren der Settings:', error);
    res.status(500).json({ success: false, error: 'Datenbankfehler' });
  }
});

// API: Vollbild-Texte abrufen
app.get('/api/fullscreen-texts', async (req, res) => {
  try {
    const result = await pool.query(
      'SELECT id, content, active, created_at FROM fullscreen_texts ORDER BY created_at DESC'
    );
    res.json({ success: true, texts: result.rows });
  } catch (error) {
    console.error('[FEHLER] beim Abrufen der Vollbild-Texte:', error);
    res.status(500).json({ success: false, error: 'Datenbankfehler' });
  }
});

// API: Vollbild-Text hinzufügen
app.post('/api/fullscreen-text', async (req, res) => {
  try {
    const { content } = req.body;
    const result = await pool.query(
      'INSERT INTO fullscreen_texts (content) VALUES ($1) RETURNING id, content, active, created_at',
      [content]
    );
    res.json({ success: true, text: result.rows[0] });
  } catch (error) {
    console.error('[FEHLER] beim Hinzufuegen:', error);
    res.status(500).json({ success: false, error: 'Datenbankfehler' });
  }
});

// API: Vollbild-Text togglen
app.put('/api/fullscreen-text/:id/toggle', async (req, res) => {
  try {
    const { id } = req.params;
    const { active } = req.body;
    const result = await pool.query(
      'UPDATE fullscreen_texts SET active = $1 WHERE id = $2 RETURNING id, content, active',
      [active, id]
    );
    res.json({ success: true, text: result.rows[0] });
  } catch (error) {
    console.error('[FEHLER] beim Togglen:', error);
    res.status(500).json({ success: false, error: 'Datenbankfehler' });
  }
});

// API: Vollbild-Text löschen
app.delete('/api/fullscreen-text/:id', async (req, res) => {
  try {
    const { id } = req.params;
    await pool.query('DELETE FROM fullscreen_texts WHERE id = $1', [id]);
    res.json({ success: true, message: 'Vollbild-Text geloescht' });
  } catch (error) {
    console.error('[FEHLER] beim Loeschen:', error);
    res.status(500).json({ success: false, error: 'Datenbankfehler' });
  }
});

// API: Aktiven Vollbild-Text abrufen
app.get('/api/fullscreen-text/latest', async (req, res) => {
  try {
    const result = await pool.query(
      'SELECT content FROM fullscreen_texts WHERE active = true LIMIT 1'
    );
    
    if (result.rows.length > 0) {
      res.json({ success: true, text: result.rows[0].content });
    } else {
      res.json({ success: true, text: null });
    }
  } catch (error) {
    console.error('[FEHLER] beim Abrufen:', error);
    res.status(500).json({ success: false, error: 'Datenbankfehler' });
  }
});

// Initialisiere alle Tabellen
initDatabase();
initSettings();
initFullscreenTexts();

// Health Check
app.get('/health', (req, res) => {
  res.json({ status: 'OK' });
});

// Root Route - Web-UI anzeigen
app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

// Server starten
app.listen(port, () => {
  console.log(`\n========================================`);
  console.log(`  Backend-Server laeuft auf Port ${port}`);
  console.log(`  Web-UI: http://localhost:${port}`);
  console.log(`  Datenbank: ${process.env.DB_HOST}:${process.env.DB_PORT}/${process.env.DB_NAME}`);
  console.log(`========================================\n`);
});

