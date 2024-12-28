#include <Wire.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <WiFi.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <driver/i2s.h>
#include <cmath>

// Informations Wi-Fi
const char* ssid = "Diego";
const char* password = "88888888";

// URL du serveur
const char* serverUrl = "http://192.168.72.161:3500";

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

// Configuration INMP441
#define I2S_WS   19  // Pin Word Select (WS)
#define I2S_SCK  18  // Pin Serial Clock (SCK)
#define I2S_SD   21  // Pin Serial Data (SD)
#define I2S_PORT I2S_NUM_0

#define SAMPLE_RATE 16000   // Taux d'échantillonnage (16 kHz)
#define BUFFER_SIZE 1024    // Taille du tampon

// Variables pour indiquer la disponibilité des capteurs
bool capteurTempDispo = true;
bool capteurPressDispo = true;

// Intervalle par défaut
unsigned long dernierEnvoi = 0;
unsigned long intervalleEnvoi = 60000; // 60 secondes

// Valeur GPS par défaut
const char* gpsDefaut = "50.559110, 5.465493";

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

  // Configuration I2S pour le microphone INMP441
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 4,
      .dma_buf_len = BUFFER_SIZE,
      .use_apll = false,
      .tx_desc_auto_clear = false,
      .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
      .bck_io_num = I2S_SCK,
      .ws_io_num = I2S_WS,
      .data_out_num = I2S_PIN_NO_CHANGE,
      .data_in_num = I2S_SD
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
  i2s_zero_dma_buffer(I2S_PORT);

  Serial.println("Microphone INMP441 configuré.");
}

void loop() {
  unsigned long tempsActuel = millis();

  // Envoi des données toutes les minutes
  if (tempsActuel - dernierEnvoi >= intervalleEnvoi) {
    dernierEnvoi = tempsActuel;
    clignoterLED();

    // Calcul du niveau sonore en dB SPL
    double niveauSonoreDBSPL = calculerNiveauSonoreDBSPL();

    // Envoyer toutes les données (capteurs + niveau sonore)
    envoyerDonnees(niveauSonoreDBSPL);
  }
}

void clignoterLED() {
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
}

double calculerNiveauSonoreDBSPL() {
  int32_t buffer[BUFFER_SIZE];
  size_t bytesRead;

  // Lecture des données audio depuis l'INMP441
  i2s_read(I2S_PORT, buffer, BUFFER_SIZE * sizeof(int32_t), &bytesRead, portMAX_DELAY);

  // Calcul du niveau RMS
  double sum = 0;
  int samples = bytesRead / sizeof(int32_t);
  for (int i = 0; i < samples; i++) {
    int32_t sample = buffer[i] >> 8; // Conversion en 24 bits
    sum += (double)(sample * sample);
  }
  double rms = sqrt(sum / samples);

  // Conversion du niveau RMS en dBFS
  double dbFS = 20.0 * log10(rms / 8388607.0); // 8388607 correspond au max pour un signal 24 bits

  // Conversion de dBFS en dB SPL
  double dbSPL = dbFS + 94.0 + 26.0; // 94 dB SPL correspond à -26 dBFS

  Serial.print("Niveau sonore (dB SPL) : ");
  Serial.println(dbSPL);

  return dbSPL;
}

void envoyerDonnees(double niveauSonoreDBSPL) {
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
  postData += "\"niveauSonoreDBSPL\":" + (isnan(niveauSonoreDBSPL) ? "null" : String(niveauSonoreDBSPL)) + ",";
  postData += "\"gps\":\"" + String(gpsDefaut) + "\"}";

  // Affichage des données JSON avant l'envoi
  Serial.println("JSON généré :");
  Serial.println(postData);

  // Envoi des données
  HTTPClient http;
  http.begin(String(serverUrl) + "/api/data");
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(postData);

  if (httpResponseCode > 0) {
    Serial.println("Données envoyées avec succès !");
  } else {
    Serial.println("Erreur lors de l'envoi des données !");
  }
  http.end();
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
  return analogRead(MQ_ANALOG_PIN);
}
