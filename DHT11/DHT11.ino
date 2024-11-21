#include <DHTesp.h>

// Définissez la broche de données du DHT11
#define DHTPIN 4  // Modifiez cette valeur en fonction de la broche utilisée

DHTesp dht;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialiser le capteur DHT11
  dht.setup(DHTPIN, DHTesp::DHT11);  // Configurer le DHT comme DHT11
  Serial.println("Initialisation du DHT11...");
}

void loop() {
  // Lire les données du capteur
  TempAndHumidity data = dht.getTempAndHumidity();

  // Vérifier si la lecture est valide
  if (isnan(data.temperature) || isnan(data.humidity)) {
    Serial.println("Erreur : Impossible de lire les données du DHT11");
  } else {
    // Afficher la température et l'humidité
    Serial.print("Température : ");
    Serial.print(data.temperature);
    Serial.println(" °C");

    Serial.print("Humidité : ");
    Serial.print(data.humidity);
    Serial.println(" %");
  }

  delay(2000); // Délai de 2 secondes entre les lectures
}
