<<<<<<< HEAD
# Station Météo - Dashboard Web

Ce projet permet de collecter des données de capteurs (telles que la température, l'humidité, la pression, le bruit, les niveaux de NOx, de CO₂, les informations UV, et les données GPS) à partir d'un microcontrôleur ESP32-S3. Toutes les composantes sont intégrées sur un PCB. Les données sont envoyées vers un serveur web créé avec Node.js, qui les stocke et les rend disponibles via une API REST. Une interface web affiche les données en utilisant des graphiques interactifs et une carte. Ce guide vous aidera à installer et à exécuter ce projet.

![Capture d'écran 2024-11-22 000724](https://github.com/user-attachments/assets/29230ce6-accf-456a-8797-8a6d42d4b178)
![Capture d'écran 2024-11-22 000749](https://github.com/user-attachments/assets/538808bd-6d15-4343-8f71-c2e874a66b47)



## Prérequis

- **Node.js** (version 14 ou ultérieure) et **npm** (généralement inclus avec Node.js).
- **ESP32-S3** préprogrammé pour envoyer les données à ce serveur.
- **Connexion Wi-Fi** active.
- **Un terminal de commande** pour exécuter des commandes Node.js.
- **Arduino IDE** pour programmer l'ESP32-S3.

## Installation des Dépendances

1. **Clônez le dépôt GitHub ou téléchargez les fichiers :**
   ```sh
   git clone https://github.com/DiegoRadigues/Weather-Station.git
   cd Station_Meteo
   ```

2. **Installez les dépendances backend :**
   Dans le répertoire du projet, exécutez la commande suivante pour installer les dépendances nécessaires pour le serveur Node.js :
   ```sh
   npm install
   ```
   Cela installera les bibliothèques suivantes :
   - **express** : Création d'un serveur HTTP.
   - **body-parser** : Parsing des requêtes entrantes avec un corps JSON.
   - **fs** et **path** : Modules Node.js pour la gestion des fichiers et des chemins.

3. **Installez les bibliothèques front-end :**
   L'interface web utilise deux bibliothèques principales :
   - **Chart.js** : Pour l'affichage des graphiques.
   - **Leaflet.js** : Pour l'affichage des informations GPS sur une carte.
   Ces bibliothèques sont chargées directement via un CDN (aucune installation locale n'est requise).

4. **Installez les bibliothèques pour Arduino IDE :**
   Pour programmer l'ESP32-S3 avec Arduino IDE, vous devrez installer les bibliothèques suivantes :
   - **ESP32 Board Package** : Ajoutez le gestionnaire de cartes ESP32 dans Arduino IDE. Allez dans `Fichier -> Préférences` et ajoutez l'URL suivante dans le champ `URL de gestionnaire de cartes supplémentaires` :
     ```
     https://dl.espressif.com/dl/package_esp32_index.json
     ```
     Ensuite, allez dans `Outils -> Type de carte -> Gestionnaire de cartes` et recherchez `ESP32` pour l'installer.
   - **Adafruit BMP085 Unified** : Bibliothèque pour utiliser le capteur BMP180 (compatible avec BMP085).
     ```sh
     Dans Arduino IDE, allez dans `Croquis -> Inclure une bibliothèque -> Gérer les bibliothèques`, puis recherchez `Adafruit BMP085 Unified` et installez-la.
     ```
   - **Adafruit Sensor** : Cette bibliothèque est requise par de nombreux capteurs, y compris le BMP180.
     ```sh
     Dans le gestionnaire de bibliothèques, recherchez `Adafruit Unified Sensor` et installez-la.
     ```
   - **WiFi.h** : Incluse automatiquement avec le paquet ESP32.
   - **HTTPClient** : Pour envoyer des requêtes HTTP, incluse avec le paquet ESP32.

## Configuration du Projet

1. **Configurer le fichier `data.json` :**
   Le serveur stocke les données envoyées par l'ESP32 dans un fichier `data.json`. Si ce fichier n'existe pas, créez-le à la racine du répertoire du projet avec le contenu suivant :
   ```json
   []
   ```
   Cela créera un tableau vide qui pourra accueillir les données de capteurs.

2. **Configurer les variables du réseau Wi-Fi sur l'ESP32 :**
   Assurez-vous que l'ESP32 est configuré avec le bon SSID et mot de passe pour qu'il puisse se connecter au réseau Wi-Fi.

## Développement Logiciel

1. **Programmation de l'ESP32-S3** :
   - L'ESP32-S3 est programmé pour lire les données des capteurs via différents protocoles (I2C, UART, SPI) et les envoyer au serveur web. Veillez à bien connecter les capteurs aux headers de la carte PCB en respectant indications sur les pins
   - **Collecte des données** : Le code Arduino sur l'ESP32-S3 lit chaque capteur à des intervalles réguliers (ex. toutes les 60 secondes) et construit un objet JSON qui est ensuite envoyé via une requête HTTP POST à l'API `/api/data` du serveur Node.js.

2. **Développement du Serveur Node.js** :
   - Le serveur utilise **Express.js** pour gérer les requêtes HTTP reçues de l'ESP32-S3.
   - Lorsqu'une requête POST est reçue sur `/api/data`, les données sont lues et ajoutées au fichier `data.json`. Ce fichier est le stockage principal pour toutes les données collectées, assurant la persistance entre les redémarrages.
   - Le serveur est également responsable de la fourniture de la page web (`index.html`) qui présente les données via des graphiques créés avec **Chart.js** et une carte avec **Leaflet.js**.

   - **Améliorations possibles** :
     - Ajouter des vérifications pour les valeurs reçues. Par exemple, ignorer les données si elles sont en dehors des plages valides.
     - Implémenter une base de données pour stocker les données, ce qui permettrait de gérer plus efficacement des volumes importants de données et d'effectuer des analyses historiques.

## Lancer le Serveur

1. **Exécuter le serveur :**
   Dans le terminal, exécutez la commande suivante pour démarrer le serveur Node.js :
   ```sh
   node server.js
   ```
   Le serveur sera lancé sur le port `3000`. Si vous souhaitez utiliser un autre port, vous pouvez modifier le fichier `server.js`.

2. **Vérifier le bon fonctionnement :**
   Ouvrez votre navigateur et allez à l'adresse suivante pour voir la page web du tableau de bord :
   ```
   http://localhost:3000
   ```

   Vous verrez une interface web affichant les données collectées sous forme de graphiques et sur une carte.

   la page
   ```
   http://localhost:3000/api/data
   ```
   Vous permettra de visualiser les données qui transitent vers votre serveur

## Utilisation de l'API

Le serveur propose deux routes principales :

- **POST `/api/data`** : Pour envoyer des données de capteurs au serveur. L'ESP32 utilise cette route pour envoyer des mesures.
  - **Exemple de payload JSON** :
    ```json
    {
      "temperature": 22.5,
      "humidité": 60.5,
      "pression": 1013.25,
      "bruit": 50.0,
      "nox": 0.05,
      "co2": 400.0,
      "gps": "48.8566,2.3522",
      "uv": 5.0
    }
    ```

- **GET `/api/data`** : Pour récupérer toutes les données enregistrées sous forme de JSON. Cette route est utilisée par l'interface web pour obtenir les informations et les afficher.

## Dépannage

- **Port occupé :** Si le port `3000` est déjà utilisé, vous pouvez changer le port dans le fichier `server.js`.
- **Problèmes Wi-Fi :** Si l'ESP32 ne parvient pas à se connecter, vérifiez le SSID et le mot de passe, ainsi que la couverture du signal.
- **Problèmes de permission sur `data.json` :** Assurez-vous que le fichier `data.json` dispose des bonnes permissions d'écriture pour que le serveur puisse le mettre à jour.
- **Dépendances manquantes :** Si une erreur survient à propos d'une dépendance manquante, exécutez `npm install` à nouveau.

## Structure du Projet

Voici la structure des fichiers pour ce projet :

```
Station_Meteo/
  |- public/
      |- index.html       # Interface web pour visualiser les données
      |- style.css            # Fichier de style pour l'interface web
  |- data.json            # Fichier de stockage des données
  |- server.js            # Serveur Node.js


## Notes Finales

Ce projet peut être étendu pour supporter davantage de capteurs ou être connecté à une base de données pour la persistance des données à long terme. Vous pouvez également améliorer l'interface web en utilisant des frameworks comme **React** ou **Vue.js**. N'hésitez pas à me partager vos améliorations

Enjoy ! 
=======
# Station Météo - Dashboard Web

Ce projet permet de collecter des données de capteurs (telles que la température, l'humidité, la pression, le bruit, les niveaux de NOx, de CO₂, les informations UV, et les données GPS) à partir d'un microcontrôleur ESP32-S3. Toutes les composantes sont intégrées sur un PCB, et l'utilisateur n'a rien à connecter manuellement. Les données sont envoyées vers un serveur web créé avec Node.js, qui les stocke et les rend disponibles via une API REST. Une interface web affiche les données en utilisant des graphiques interactifs et une carte. Ce guide vous aidera à installer et à exécuter ce projet.

## Prérequis

- **Node.js** (version 14 ou ultérieure) et **npm** (généralement inclus avec Node.js).
- **ESP32-S3** préprogrammé pour envoyer les données à ce serveur.
- **Connexion Wi-Fi** active.
- **Un terminal de commande** pour exécuter des commandes Node.js.
- **Arduino IDE** pour programmer l'ESP32-S3.

## Installation des Dépendances

1. **Clônez le dépôt GitHub ou téléchargez les fichiers :**
   ```sh
   git clone https://github.com/DiegoRadigues/Weather-Station.git
   cd Station_Meteo
   ```

2. **Installez les dépendances backend :**
   Dans le répertoire du projet, exécutez la commande suivante pour installer les dépendances nécessaires pour le serveur Node.js :
   ```sh
   npm install
   ```
   Cela installera les bibliothèques suivantes :
   - **express** : Création d'un serveur HTTP.
   - **body-parser** : Parsing des requêtes entrantes avec un corps JSON.
   - **fs** et **path** : Modules Node.js pour la gestion des fichiers et des chemins.

3. **Installez les bibliothèques front-end :**
   L'interface web utilise deux bibliothèques principales :
   - **Chart.js** : Pour l'affichage des graphiques.
   - **Leaflet.js** : Pour l'affichage des informations GPS sur une carte.
   Ces bibliothèques sont chargées directement via un CDN (aucune installation locale n'est requise).

4. **Installez les bibliothèques pour Arduino IDE :**
   Pour programmer l'ESP32-S3 avec Arduino IDE, vous devrez installer les bibliothèques suivantes :
   - **ESP32 Board Package** : Ajoutez le gestionnaire de cartes ESP32 dans Arduino IDE. Allez dans `Fichier -> Préférences` et ajoutez l'URL suivante dans le champ `URL de gestionnaire de cartes supplémentaires` :
     ```
     https://dl.espressif.com/dl/package_esp32_index.json
     ```
     Ensuite, allez dans `Outils -> Type de carte -> Gestionnaire de cartes` et recherchez `ESP32` pour l'installer.
   - **Adafruit BMP085 Unified** : Bibliothèque pour utiliser le capteur BMP180 (compatible avec BMP085).
     ```sh
     Dans Arduino IDE, allez dans `Croquis -> Inclure une bibliothèque -> Gérer les bibliothèques`, puis recherchez `Adafruit BMP085 Unified` et installez-la.
     ```
   - **Adafruit Sensor** : Cette bibliothèque est requise par de nombreux capteurs, y compris le BMP180.
     ```sh
     Dans le gestionnaire de bibliothèques, recherchez `Adafruit Unified Sensor` et installez-la.
     ```
   - **WiFi.h** : Incluse automatiquement avec le paquet ESP32.
   - **HTTPClient** : Pour envoyer des requêtes HTTP, incluse avec le paquet ESP32.

## Configuration du Projet

1. **Configurer le fichier `data.json` :**
   Le serveur stocke les données envoyées par l'ESP32 dans un fichier `data.json`. Si ce fichier n'existe pas, créez-le à la racine du répertoire du projet avec le contenu suivant :
   ```json
   []
   ```
   Cela créera un tableau vide qui pourra accueillir les données de capteurs.

2. **Configurer les variables du réseau Wi-Fi sur l'ESP32 :**
   Assurez-vous que l'ESP32 est configuré avec le bon SSID et mot de passe pour qu'il puisse se connecter au réseau Wi-Fi.

## Développement Logiciel

1. **Programmation de l'ESP32-S3** :
   - L'ESP32-S3 est programmé pour lire les données des capteurs via différents protocoles (I2C, UART, SPI) et les envoyer au serveur web. Tous les capteurs sont déjà intégrés sur le PCB, donc aucune connexion manuelle n'est requise.
   - **Collecte des données** : Le code Arduino sur l'ESP32-S3 lit chaque capteur à des intervalles réguliers (ex. toutes les 60 secondes) et construit un objet JSON qui est ensuite envoyé via une requête HTTP POST à l'API `/api/data` du serveur Node.js.
   - **Gestion de l'alimentation et veille** : Pour optimiser la consommation d'énergie, vous pouvez utiliser le mode de veille profonde de l'ESP32 entre les cycles de mesure des capteurs. Cela est particulièrement utile si le projet est destiné à une utilisation en extérieur alimentée par une batterie.

2. **Développement du Serveur Node.js** :
   - Le serveur utilise **Express.js** pour gérer les requêtes HTTP reçues de l'ESP32-S3.
   - Lorsqu'une requête POST est reçue sur `/api/data`, les données sont lues et ajoutées au fichier `data.json`. Ce fichier est le stockage principal pour toutes les données collectées, assurant la persistance entre les redémarrages.
   - Le serveur est également responsable de la fourniture de la page web (`index.html`) qui présente les données via des graphiques créés avec **Chart.js** et une carte avec **Leaflet.js**.

   - **Améliorations possibles** :
     - Ajouter des vérifications pour les valeurs reçues. Par exemple, ignorer les données si elles sont en dehors des plages valides.
     - Implémenter une base de données (comme **MongoDB**) pour stocker les données, ce qui permettrait de gérer plus efficacement des volumes importants de données et d'effectuer des analyses historiques.

3. **Interface Utilisateur Web** :
   - Le fichier `index.html` utilise **Chart.js** pour visualiser la température, l'humidité, la pression, le niveau de bruit, et d'autres mesures sous forme de graphiques interactifs.
   - Une carte est générée avec **Leaflet.js** pour afficher les informations GPS et localiser la station météo en temps réel.
   - Les données sont récupérées en utilisant l'API **GET `/api/data`**, puis sont mises à jour automatiquement toutes les minutes pour assurer que l'utilisateur voit toujours les dernières informations collectées.

   - **JavaScript pour Actualiser les Données** :
     ```js
     async function fetchData() {
       const response = await fetch('/api/data');
       const data = await response.json();
       // Mise à jour des graphiques et de la carte avec les nouvelles données
     }

     setInterval(fetchData, 60000); // Actualiser les données toutes les 60 secondes
     ```

## Lancer le Serveur

1. **Exécuter le serveur :**
   Dans le terminal, exécutez la commande suivante pour démarrer le serveur Node.js :
   ```sh
   node server.js
   ```
   Le serveur sera lancé sur le port `3000`. Si vous souhaitez utiliser un autre port, vous pouvez modifier le fichier `server.js`.

2. **Vérifier le bon fonctionnement :**
   Ouvrez votre navigateur et allez à l'adresse suivante pour voir la page web du tableau de bord :
   ```
   http://localhost:3000
   ```

   Vous verrez une interface web affichant les données collectées sous forme de graphiques et sur une carte.

## Utilisation de l'API

Le serveur propose deux routes principales :

- **POST `/api/data`** : Pour envoyer des données de capteurs au serveur. L'ESP32 utilise cette route pour envoyer des mesures.
  - **Exemple de payload JSON** :
    ```json
    {
      "temperature": 22.5,
      "humidité": 60.5,
      "pression": 1013.25,
      "bruit": 50.0,
      "nox": 0.05,
      "co2": 400.0,
      "gps": "48.8566,2.3522",
      "uv": 5.0
    }
    ```

- **GET `/api/data`** : Pour récupérer toutes les données enregistrées sous forme de JSON. Cette route est utilisée par l'interface web pour obtenir les informations et les afficher.

## Dépannage

- **Port occupé :** Si le port `3000` est déjà utilisé, vous pouvez changer le port dans le fichier `server.js`.
- **Problèmes Wi-Fi :** Si l'ESP32 ne parvient pas à se connecter, vérifiez le SSID et le mot de passe, ainsi que la couverture du signal.
- **Problèmes de permission sur `data.json` :** Assurez-vous que le fichier `data.json` dispose des bonnes permissions d'écriture pour que le serveur puisse le mettre à jour.
- **Dépendances manquantes :** Si une erreur survient à propos d'une dépendance manquante, exécutez `npm install` à nouveau.

## Structure du Projet

Voici la structure des fichiers pour ce projet :

```
Station_Meteo/
  |- public/
      |- index.html       # Interface web pour visualiser les données
  |- data.json            # Fichier de stockage des données
  |- server.js            # Serveur Node.js
  |- style.css            # Fichier de style pour l'interface web
  |- package.json         # Fichier de dépendances Node.js
```

## Notes Finales

Ce projet peut être étendu pour supporter davantage de capteurs ou être connecté à une base de données pour la persistance des données à long terme. Vous pouvez également améliorer l'interface web en utilisant des frameworks comme **React** ou **Vue.js**.

Enjoy ! 
>>>>>>> afba63d (Initial commit or updated project version)
