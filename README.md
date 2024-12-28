
# **Station Météo - Dashboard Web**

Ce projet permet de collecter des données provenant de capteurs (température, humidité, pression, bruit, qualité de l'air, informations UV et GPS) via un microcontrôleur **ESP32-S3**. Toutes les composantes sont intégrées sur un **PCB**, évitant toute connexion manuelle. Les données sont envoyées vers un serveur web créé avec **Node.js**, qui les stocke et les rend disponibles via une **API REST**. Une interface web permet de visualiser les données sous forme de graphiques interactifs et sur une carte.

![StationUI](https://github.com/user-attachments/assets/682e8851-fc01-4cb2-aaad-4d59fbfb2464)

---

## **Prérequis**

### **1. Bibliothèques Arduino IDE :**
Pour programmer l'ESP32-S3, les bibliothèques suivantes doivent être installées :

1. **Adafruit Unified Sensor**  
2. **Adafruit BMP085 Unified**  
3. **DHT sensor library**  
4. **ArduinoJson**
5. **cmath**

## **Incluses avec le package ESP32 :**
- **Wire**  
- **WiFi**  
- **HTTPClient**  
- **driver/i2s.h**  


### **2. Matériel nécessaire :**
- **ESP32-S3** : Carte microcontrôleur principale.
- **Capteur BMP180** : Pour mesurer la température et la pression atmosphérique.
- **Capteur DHT11** : Pour mesurer la température et l'humidité relative.
- **Capteur MQ** : Pour surveiller la qualité de l'air (valeur analogique).
- **Microphone INMP441** : Pour mesurer le niveau sonore en dB SPL.

### **3. Configuration réseau :**
- **Wi-Fi 2.4 GHz** : Réseau compatible avec l'ESP32-S3.
- **Serveur HTTP actif** sur l'IP spécifiée (ex. `192.168.72.161`) et le port **3500**.



---

## **Installation des Dépendances**

### **1. Cloner le dépôt GitHub :**
```sh
git clone https://github.com/DiegoRadigues/Weather-Station.git
cd Station_Meteo
```

### **2. Installer les dépendances backend :**
```sh
npm install
```
**Bibliothèques installées :**  
- **express** : Serveur HTTP.  
- **body-parser** : Analyse des requêtes JSON entrantes.  
- **fs** et **path** : Gestion des fichiers et chemins.  

### **3. Installer les bibliothèques front-end :**
Aucune installation locale requise. Les bibliothèques sont chargées via **CDN** :  
- **Chart.js** : Graphiques interactifs.  
- **Leaflet.js** : Cartographie GPS.  

### **4. Installer les bibliothèques pour Arduino IDE :**
1. **ESP32 Board Package** :  
   - Préférences → Ajouter cette URL dans *"URL gestionnaire de cartes supplémentaires"* :  
     ```
     https://dl.espressif.com/dl/package_esp32_index.json
     ```
   - Installer **ESP32** dans *Gestionnaire de cartes*.  
2. **Adafruit BMP085 Unified** :  
   - Croquis → Inclure une bibliothèque → Gérer les bibliothèques.  
   - Rechercher **Adafruit BMP085 Unified** et l'installer.  
3. **Adafruit Unified Sensor** :  
   - Même procédure pour installer **Adafruit Unified Sensor**.  
4. **WiFi.h** : Déjà inclus dans le package **ESP32**.  
5. **HTTPClient** : Inclus également avec le package **ESP32**.  

---

## **Configuration du Projet**

### **1. Fichier `data.json` :**
Créer un fichier vide à la racine pour stocker les données :  
```json
[]
```

### **2. Configuration Wi-Fi sur l'ESP32 :**
Mettre à jour le code pour inclure le **SSID** et le **mot de passe** corrects dans la configuration Wi-Fi.  

---

## **Développement Logiciel**

### **1. Programmation de l'ESP32-S3** :
- Collecte des données des capteurs via **I2C, UART, SPI**.  
- Envoi des données en JSON au serveur via une requête **HTTP POST** sur `/api/data`.  
- Exemple de données envoyées :  
  ```json
  [
    {
        "temperatureBMP": 22.7,
        "temperatureDHT": 23.3,
        "pression": 1022.57,
        "humidite": 58,
        "MQ_analogique": 781,
        "niveauSonoreDBSPL": 43.25,
        "gps": "50.559110, 5.465493",
        "timestamp": "2024-12-27T13:31:56.985Z"
    }
  ]
  ```
- **Optimisation énergétique** : Implémenter la **veille profonde** pour économiser l'énergie entre les cycles de mesure.

### **2. Développement du serveur Node.js :**
- **Express.js** gère les requêtes HTTP.  
- Sauvegarde des données dans `data.json` pour persistance.  
- Sert la page web interactive avec **Chart.js** et **Leaflet.js**.  

**Améliorations possibles** :  
- Vérifier la validité des données reçues.  
- Implémenter une base de données (ex. **MongoDB**) pour un stockage plus robuste.  

---

## **Lancer le Serveur**

### **1. Démarrer le serveur :**
```sh
node server.js
```
Le serveur fonctionne sur le port **3500** par défaut. Modifier le fichier **server.js** pour changer de port si nécessaire.  

### **2. Vérifier le fonctionnement :**
Accéder au tableau de bord :  
```
http://localhost:3500
```
Visualiser les données collectées :  
```
http://localhost:3500/api/data
```

---

## **Utilisation de l'API**

### **Routes principales :**
1. **POST `/api/data`** : Reçoit des mesures depuis l'ESP32.  
2. **GET `/api/data`** : Renvoie toutes les données stockées au format JSON.  

---

## **Dépannage**

- **Port 3500 occupé** : Modifier dans **server.js**.  
- **Wi-Fi non fonctionnel** : Vérifier le **SSID**, mot de passe et s'assurer du 2.4 GHz.  
- **Problèmes avec `data.json`** : Vérifier les permissions d'écriture.  
- **Erreur de dépendances** : Réexécuter :  
  ```sh
  npm install
  ```

---

## **Structure du Projet**

```
PCB advanced/
│   data.json           # Stockage des données des capteurs
│   esp32_script.cpp    # Script C++ pour programmer l'ESP32-S3
│   README.md           # Documentation du projet
│   server.js           # Serveur Node.js pour la gestion des données
│
├───public/             # Dossier pour l'interface web
│       index.html      # Page web principale
│       script.js       # Scripts JavaScript pour la logique front-end
│       style.css       # Feuille de style pour la mise en page
│
└───script/             # Dossier pour les scripts Arduino
        script.ino      # Code Arduino pour l'ESP32-S3
```

---

## **Notes Finales**

Ce projet peut être étendu pour :  
- Ajouter des capteurs supplémentaires.  
- Utiliser une base de données (ex. **MongoDB**).  
- Améliorer l'interface web avec des frameworks comme **React** ou **Vue.js**.  
- Optimiser la gestion énergétique pour une utilisation en extérieur sur batterie.

N'hésitez pas à proposer des améliorations et **enjoy !** 
