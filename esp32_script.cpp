#include <WiFi.h>
#include <HTTPClient.h>


const char* ssid = "Votre_SSID";
const char* password = "Votre_MotDePasse";
const char* serverUrl = "http://votre_serveur:3000/api/data";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
 

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion au Wi-Fi en cours...");
  }
  Serial.println("Connecté au Wi-Fi");
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

    // Créer JSON
    String postData = "{";
    postData += "\"temperature\":" + String(temperature) + ",";
    postData += "\"pression\":" + String(pression) + ",";
    postData += "\"bruit\":" + String(bruit) + ",";
    postData += "\"nox\":" + String(nox) + ",";
    postData += "\"co2\":" + String(co2) + ",";
    postData += "\"gps\":\"" + gps + "\",";
    postData += "\"uv\":" + String(uv);
    postData += "}";

    // Envoyer les data au serveur
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Réponse du serveur: " + response);
    } else {
      Serial.println("Erreur lors de l'envoi des données");
    }
    http.end();
  } else {
    Serial.println("Wi-Fi non connecté");
  }
  delay(60000); // freq 60 secondes
}
