document.addEventListener('DOMContentLoaded', () => {

  // Fonction pour afficher une erreur
  function afficherErreur(elementId, message) {
    const element = document.getElementById(elementId);
    element.textContent = message;
    element.style.color = "red";
  }

  // Fonction pour afficher un message temporaire
  function afficherMessageTemporaire(elementId, message, duree = 3000) {
    const element = document.getElementById(elementId);
    element.textContent = message;
    setTimeout(() => {
      element.textContent = "";
    }, duree);
  }

  // Fonction pour mettre à jour le statut de l'ESP
  function mettreAJourStatus() {
    fetch('/api/status')
      .then(resp => resp.json())
      .then(data => {
        console.log("Statut reçu :", data);
        const statusElement = document.getElementById('esp-status');
        if (data.status === "online") {
          statusElement.textContent = `Connecté (Dernier contact : ${new Date(data.dernierContact).toLocaleString()})`;
          statusElement.style.color = "green";
        } else {
          statusElement.textContent = "Hors ligne";
          statusElement.style.color = "red";
        }
      })
      .catch(() => {
        afficherErreur('esp-status', "Erreur de connexion au serveur");
      });
  }

  // Fonction pour créer un graphique
  function creerGraphique(id, label, dataKey, color, data) {
    const filteredData = data.filter(entry => entry[dataKey] !== null);
    const labels = filteredData.map(entry => new Date(entry.timestamp).toLocaleString());
    const values = filteredData.map(entry => entry[dataKey]);

    const ctx = document.getElementById(id).getContext('2d');
    new Chart(ctx, {
      type: 'line',
      data: {
        labels: labels,
        datasets: [{
          label: label,
          data: values,
          borderColor: color,
          fill: false,
        }]
      },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        plugins: {
          legend: { display: true },
        }
      }
    });
  }

  // Fonction pour récupérer les données et générer les graphiques
  function mettreAJourGraphiques() {
    fetch('/api/data')
      .then(resp => resp.json())
      .then(data => {
        console.log("Données reçues pour les graphiques :", data);

        // Création des graphiques
        creerGraphique('temperatureChart', 'Température BMP (°C)', 'temperatureBMP', 'rgba(255, 99, 132, 1)', data);
        creerGraphique('pressionChart', 'Pression (hPa)', 'pression', 'rgba(54, 162, 235, 1)', data);
        creerGraphique('humiditéChart', 'Humidité (%)', 'humidite', 'rgba(75, 192, 192, 1)', data);
        creerGraphique('gazChart', 'Gaz MQ (Analogique)', 'MQ_analogique', 'rgba(153, 102, 255, 1)', data);
      })
      .catch(() => {
        afficherErreur('esp-status', "Erreur lors de la récupération des données pour les graphiques.");
      });
  }

  // Fonction pour afficher la carte
  function afficherCarte(data) {
    const map = L.map('mapid').setView([50.845693, 4.393563], 13); // Coordonnées par défaut si aucune donnée

    L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
      maxZoom: 19,
      attribution: '© OpenStreetMap contributors',
    }).addTo(map);

    // Ajouter un marqueur si les données GPS sont disponibles
    if (data.gps) {
      const [latitude, longitude] = data.gps.split(',').map(coord => parseFloat(coord.trim()));
      L.marker([latitude, longitude]).addTo(map)
        .bindPopup(`Données GPS : ${latitude}, ${longitude}`)
        .openPopup();
    }
  }
    function mettreAJourGraphiques() {
      fetch('/api/data')
        .then(resp => resp.json())
        .then(data => {
          console.log("Données reçues pour les graphiques :", data);
    
          // Création des graphiques pour chaque mesure
          creerGraphique('temperatureChart', 'Température BMP (°C)', 'temperatureBMP', 'rgba(255, 99, 132, 1)', data);
          creerGraphique('pressionChart', 'Pression (hPa)', 'pression', 'rgba(54, 162, 235, 1)', data);
          creerGraphique('gazChart', 'Gaz MQ X', 'MQ_analogique', 'rgba(153, 102, 255, 1)', data);
        })
        .catch(() => {
          afficherErreur('esp-status', "Erreur lors de la récupération des données pour les graphiques.");
        });
    }
  
  

  // Récupérer les données et afficher la carte
  fetch('/api/data')
    .then(resp => resp.json())
    .then(data => {
      const latestData = data[data.length - 1]; // Dernière donnée reçue
      afficherCarte(latestData);
    })
    .catch(() => {
      console.error('Erreur lors de la récupération des données pour la carte.');
    });

  // Déclencher une mesure immédiate
  document.getElementById('trigger-measurement-button').addEventListener('click', () => {
    fetch('/api/trigger', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
    })
      .then(resp => resp.json())
      .then(data => {
        afficherMessageTemporaire('trigger-status', data.message || "Mesure déclenchée avec succès !");
      })
      .catch(() => {
        afficherErreur('trigger-status', "Erreur lors du déclenchement de la mesure.");
      });
  });

  // Initialiser les mises à jour automatiques
  setInterval(mettreAJourStatus, 5000);
  setInterval(mettreAJourGraphiques, 30000); // Met à jour les graphiques toutes les 30 secondes

  // Appels initiaux
  mettreAJourStatus();
  mettreAJourGraphiques();
});
