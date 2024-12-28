const express = require('express');
const bodyParser = require('body-parser');
const fs = require('fs');
const path = require('path');
const app = express();

// Middleware
app.use(bodyParser.json());
app.use(express.static('public'));

// Variables globales
let dernierContactESP = null;
let mesureForcee = false;

// Fichier de données
const dataFilePath = path.join(__dirname, 'data.json');

// Durée maximale avant de considérer l'ESP comme hors ligne (en secondes)
const ESP_TIMEOUT_SECONDS = 120;
// Période tampon pour éviter les oscillations (en secondes)
const GRACE_PERIOD_SECONDS = 10;

// Création automatique de `data.json` si inexistant
if (!fs.existsSync(dataFilePath)) {
  fs.writeFileSync(dataFilePath, JSON.stringify([]));
  console.log("Fichier data.json créé automatiquement.");
}

// Routes
// Page principale
app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

// Endpoint pour recevoir les données
app.post('/api/data', (req, res) => {
  console.log("Données reçues :", req.body);

  // Validation des données
  const { temperatureBMP, temperatureDHT, pression, humidite, MQ_analogique, niveauSonoreDBSPL, gps } = req.body;

  const isValidNumber = (value) => typeof value === 'number' && !isNaN(value);
  const isValidString = (value) => typeof value === 'string' && value.trim() !== '';

  // Vérification individuelle des champs
  const errors = [];
  if (!isValidNumber(temperatureBMP)) errors.push("temperatureBMP");
  if (!isValidNumber(temperatureDHT)) errors.push("temperatureDHT");
  if (!isValidNumber(pression)) errors.push("pression");
  if (!isValidNumber(humidite)) errors.push("humidite");
  if (!isValidNumber(MQ_analogique)) errors.push("MQ_analogique");
  if (!isValidNumber(niveauSonoreDBSPL)) errors.push("niveauSonoreDBSPL");
  if (!isValidString(gps)) errors.push("gps");

  // Si toutes les données sont invalides, rejeter la requête
  if (errors.length === Object.keys(req.body).length) {
    console.error("Toutes les données reçues sont invalides ou absentes :", req.body);
    return res.status(400).json({ error: "Toutes les données sont invalides ou absentes", details: errors });
  }

  // Mise à jour du dernier contact
  dernierContactESP = new Date().toISOString();

  // Lecture et mise à jour des données dans le fichier JSON
  fs.readFile(dataFilePath, 'utf8', (err, data) => {
    if (err) {
      console.error("Erreur de lecture du fichier JSON :", err.message);
      return res.status(500).send('Erreur de lecture des données');
    }

    let jsonData;
    try {
      jsonData = JSON.parse(data);
    } catch (e) {
      console.error("Erreur de parsing JSON :", e.message);
      return res.status(500).send('Erreur de parsing du fichier JSON');
    }

    // Ajout des nouvelles données
    const newData = {
      temperatureBMP: isValidNumber(temperatureBMP) ? temperatureBMP : null,
      temperatureDHT: isValidNumber(temperatureDHT) ? temperatureDHT : null,
      pression: isValidNumber(pression) ? pression : null,
      humidite: isValidNumber(humidite) ? humidite : null,
      MQ_analogique: isValidNumber(MQ_analogique) ? MQ_analogique : null,
      niveauSonoreDBSPL: isValidNumber(niveauSonoreDBSPL) ? niveauSonoreDBSPL : null,
      gps: isValidString(gps) ? gps : "50.84569334668039, 4.39356337571457",
      timestamp: new Date().toISOString(),
    };

    jsonData.push(newData);

    // Limite de stockage des données
    if (jsonData.length > 3000) {
      jsonData.shift();
    }

    // Écriture des données mises à jour
    fs.writeFile(dataFilePath, JSON.stringify(jsonData, null, 2), (err) => {
      if (err) {
        console.error("Erreur lors de l'écriture des données :", err.message);
        return res.status(500).send('Erreur lors de l\'écriture des données');
      }
      console.log("Données enregistrées avec succès :", newData);
      res.status(200).json({ message: 'Données enregistrées', reloadGraph: true });
    });
  });
});

// Endpoint pour récupérer les données
app.get('/api/data', (req, res) => {
  fs.readFile(dataFilePath, 'utf8', (err, data) => {
    if (err) {
      console.error("Erreur de lecture des données :", err.message);
      return res.json([]);
    }

    try {
      const jsonData = JSON.parse(data);
      res.json(jsonData);
    } catch (e) {
      console.error("Erreur de parsing JSON :", e.message);
      res.status(500).send('Erreur de parsing du fichier JSON');
    }
  });
});

// Endpoint pour récupérer le statut de l'ESP
app.get('/api/status', (req, res) => {
  if (!dernierContactESP) {
    console.log("[Status] ESP jamais connecté.");
    return res.json({ status: "offline", dernierContact: null });
  }

  const maintenant = new Date();
  const dernierContact = new Date(dernierContactESP);
  const tempsEcoule = (maintenant - dernierContact) / 1000;

  console.log(`[Status] Temps écoulé depuis le dernier contact : ${tempsEcoule} secondes.`);

  if (tempsEcoule > ESP_TIMEOUT_SECONDS + GRACE_PERIOD_SECONDS) {
    console.log("[Status] ESP hors ligne (timeout dépassé).");
    return res.json({ status: "offline", dernierContact: dernierContactESP });
  }

  console.log("[Status] ESP en ligne.");
  res.json({ status: "online", dernierContact: dernierContactESP });
});

// Endpoint pour déclencher une mesure immédiate
app.post('/api/trigger', (req, res) => {
  console.log("Mesure immédiate déclenchée !");
  mesureForcee = true;
  res.status(200).json({ message: "Mesure immédiate déclenchée." });
});

app.get('/api/trigger', (req, res) => {
  res.json({ trigger: mesureForcee });
  mesureForcee = false;
});

// Middleware global pour les erreurs
app.use((err, req, res, next) => {
  console.error(err.stack);
  res.status(500).send('Erreur serveur interne.');
});

// Lancement du serveur
const PORT = 3500;
app.listen(PORT, () => {
  console.log(`Serveur démarré sur le port ${PORT}`);
});
