#include <Wire.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <WiFi.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Informations Wi-Fi
const char* ssid = "Diego";
const char* password = "88888888";

// URL du serveur
const char* serverUrl = "http://192.168.140.161:3000";

// Utilisation des broches GPIO 9 pour SCL et GPIO 8 pour SDA
#define SDA_PIN 8
#define SCL_PIN 9

// Configuration du DHT11
#define DHTPIN 10
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// LED intégrée
#define LED_PIN 2

// Configuration du capteur MQ
#define MQ_ANALOG_PIN 5  // Entrée analogique pour le capteur MQ (AO) sur GPIO5

// BMP180
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

// Variables pour indiquer la disponibilité des capteurs
bool capteurTempDispo = true;
bool capteurPressDispo = true;
bool capteurHumiditeDispo = true;

// Intervalle par défaut
unsigned long dernierEnvoi = 0;
unsigned long intervalleEnvoi = 60000; // 60 secondes

// Valeur GPS par défaut
const char* gpsDefaut = "50.84569334668039, 4.39356337571457";

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Connexion Wi-Fi
  Serial.print("Connexion au Wi-Fi : ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnecté au Wi-Fi!");

  // Initialisation des broches
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Initialisation des capteurs
  Wire.begin(SDA_PIN, SCL_PIN);
  dht.begin();

  Serial.println("Initialisation des capteurs...");
  if (!bmp.begin()) {
    Serial.println("Erreur : Impossible de trouver le capteur BMP180.");
    capteurTempDispo = false;
    capteurPressDispo = false;
  }
}

void loop() {
  unsigned long tempsActuel = millis();

  // Vérifie si une mesure immédiate est demandée
  if (verifierMesureForcee()) {
    clignoterLED();
    envoyerDonnees();
  }

  // Vérifie si le délai entre les mesures est écoulé
  if (tempsActuel - dernierEnvoi >= intervalleEnvoi) {
    dernierEnvoi = tempsActuel;
    clignoterLED();
    envoyerDonnees();
  }

  delay(1000);
}

void clignoterLED() {
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
}

void envoyerDonnees() {
  if (WiFi.status() != WL_CONNECTED) return;

  float temperatureBMP = capteurTempDispo ? lireTemperatureBMP() : NAN;
  float temperatureDHT = lireTemperatureDHT11();
  float pression = capteurPressDispo ? lirePression() : NAN;
  float humidite = lireHumiditeDHT11();
  float gazAnalogique = lireGazAnalogique();

  // Construction JSON
  String postData = "{";
  postData += "\"temperatureBMP\":" + (isnan(temperatureBMP) ? "null" : String(temperatureBMP)) + ",";
  postData += "\"temperatureDHT\":" + (isnan(temperatureDHT) ? "null" : String(temperatureDHT)) + ",";
  postData += "\"pression\":" + (isnan(pression) ? "null" : String(pression)) + ",";
  postData += "\"humidite\":" + (isnan(humidite) ? "null" : String(humidite)) + ",";
  postData += "\"MQ_analogique\":" + String(gazAnalogique) + ",";
  postData += "\"gps\":\"" + String(gpsDefaut) + "\"";
  postData += "}";

  HTTPClient http;
  http.begin(String(serverUrl) + "/api/data");
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(postData);

  if (httpResponseCode > 0) {
    Serial.println("Données envoyées : " + postData);
  } else {
    Serial.println("Erreur d'envoi des données.");
  }
  http.end();
}

bool verifierMesureForcee() {
  HTTPClient http;
  http.begin(String(serverUrl) + "/api/trigger");
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    String payload = http.getString();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);
    return doc["trigger"].as<bool>();
  }
  return false;
}

float lireTemperatureBMP() {
  float temperature;
  bmp.getTemperature(&temperature);
  return temperature;
}

float lireTemperatureDHT11() {
  return dht.readTemperature();
}

float lireHumiditeDHT11() {
  return dht.readHumidity();
}

float lirePression() {
  sensors_event_t event;
  bmp.getEvent(&event);
  return event.pressure;
}

float lireGazAnalogique() {
  return analogRead(MQ_ANALOG_PIN); // Lecture de la sortie analogique
}