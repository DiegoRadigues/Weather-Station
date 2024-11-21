#include <WiFi.h>
#include <HTTPClient.h>

// Remplacez par vos informations Wi-Fi
const char* ssid = "Diego";
const char* password = "88888888";

const char* serverUrl = "http://192.168.54.161:3000/api/data";


// Variables pour indiquer la disponibilité des capteurs
bool capteurTempDispo = true;
bool capteurPressDispo = true;
bool capteurBruitDispo = true;
bool capteurNOxDispo = true;
bool capteurCO2Dispo = true;
bool capteurGPSDispo = true;
bool capteurUVDispo = true;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Initialisation des capteurs (à implémenter selon vos capteurs)
  // Exemple :
  // if (initialisation_capteur_temp()) {
  //   capteurTempDispo = true;
  // } else {
  //   capteurTempDispo = false;
  // }

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    float temperature = lireTemperature();
    float pression = lirePression();
    float bruit = lireBruit();
    float nox = lireNOx();
    float co2 = lireCO2();
    String gps = lireGPS();
    float uv = lireUV();

    String postData = "{";

    if (!isnan(temperature)) {
      postData += "\"temperature\":" + String(temperature) + ",";
    } else {
      postData += "\"temperature\":null,";
    }

    if (!isnan(pression)) {
      postData += "\"pression\":" + String(pression) + ",";
    } else {
      postData += "\"pression\":null,";
    }

    if (!isnan(bruit)) {
      postData += "\"bruit\":" + String(bruit) + ",";
    } else {
      postData += "\"bruit\":null,";
    }

    if (!isnan(nox)) {
      postData += "\"nox\":" + String(nox) + ",";
    } else {
      postData += "\"nox\":null,";
    }

    if (!isnan(co2)) {
      postData += "\"co2\":" + String(co2) + ",";
    } else {
      postData += "\"co2\":null,";
    }

    if (gps != "") {
      postData += "\"gps\":\"" + gps + "\",";
    } else {
      postData += "\"gps\":null,";
    }

    if (!isnan(uv)) {
      postData += "\"uv\":" + String(uv) + ",";
    } else {
      postData += "\"uv\":null,";
    }

    if (postData.endsWith(",")) {
      postData.remove(postData.length() - 1);
    }

    postData += "}";

    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(postData);
    http.end();
  }
  delay(60000);
}

float lireTemperature() {
  if (capteurTempDispo) {
    // Code pour lire le capteur de température
    // Retourner la valeur lue
    return 25.0; // Valeur simulée
  } else {
    return NAN;
  }
}

float lirePression() {
  if (capteurPressDispo) {
    // Code pour lire le capteur de pression
    return 1013.25; // Valeur simulée
  } else {
    return NAN;
  }
}

float lireBruit() {
  if (capteurBruitDispo) {
    // Code pour lire le capteur de bruit
    return 50.0; // Valeur simulée
  } else {
    return NAN;
  }
}

float lireNOx() {
  if (capteurNOxDispo) {
    // Code pour lire le capteur NOx
    return 0.05; // Valeur simulée
  } else {
    return NAN;
  }
}

float lireCO2() {
  if (capteurCO2Dispo) {
    // Code pour lire le capteur CO2
    return 400.0; // Valeur simulée
  } else {
    return NAN;
  }
}

String lireGPS() {
  if (capteurGPSDispo) {
    // Code pour lire le GPS
    return "48.8566,2.3522"; // Valeur simulée (coordonnées de Paris)
  } else {
    return "";
  }
}

float lireUV() {
  if (capteurUVDispo) {
    // Code pour lire le capteur UV
    return 5.0; // Valeur simulée
  } else {
    return NAN;
  }
}

