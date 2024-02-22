#include "WiFi.h"
#include <U8x8lib.h>
#include <PubSubClient.h>

// Paramètres de connexion WiFi
const char* ssid = "raspi-webgui";
const char* password = "RASPI_TP";

// Paramètres de connexion MQTT
const char* mqtt_server = "10.3.141.205";
const int mqtt_port = 1883;
const char* mqtt_topic = "srt5/midi";
const char* message_text = "MIDI2;867;9;125"; // Message envoyé

// Initialisation de l'écran OLED
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(15, 4, 16);

// Client WiFi pour la connexion au réseau
WiFiClient espClient;
// Client MQTT
PubSubClient client(espClient);

unsigned long lastMsgTime = 0; // Temps auquel le dernier message a été envoyé
const long interval = 100000;   // Intervalle de temps entre les messages (10 secondes)

void setup() {
  // Initialiser le port série pour le débogage
  Serial.begin(115200);

  // Configuration du mode WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Initialisation de l'écran OLED
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  // Connexion au réseau WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    u8x8.drawString(0, 0, "Connection...");
  }

  // Affichage de l'adresse IP une fois connecté
  u8x8.drawString(0, 0, "WiFi Connecté");
  u8x8.drawString(0, 1, WiFi.localIP().toString().c_str());

  // Configuration des paramètres du serveur MQTT et la fonction de callback
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Connexion au serveur MQTT
  if (client.connect("MIDI2")) {
    u8x8.drawString(0, 2, "MQTT Connecté");
    client.subscribe(mqtt_topic);
  }
}



void loop() {
  // Maintien de la connexion au serveur MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Vérifier si l'intervalle de temps s'est écoulé avant d'envoyer un nouveau message
  unsigned long now = millis();
  if (now - lastMsgTime > interval) {
    lastMsgTime = now;
    
    // sendMessage();    // Envoi du message
  }
}












void sendMessage() {
  // Verification de la connexion avant envoi
  if (client.connected()) {
    client.publish(mqtt_topic, message_text);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Traduction du message reçu
  char message[length + 1];
  strncpy(message, (char*)payload, length);
  message[length] = '\0'; // Fin de chaîne

  // Message et topic reçu sur l'écran OLED
  u8x8.drawString(0, 3, "MQTT Topic:");
  u8x8.drawString(0, 4, topic);
  u8x8.drawString(0, 5, "MSG:");
  u8x8.drawString(0, 6, message);

  // Extraction des valeurs fréquence, spreadFactor et bandwidth après le saut de ligne
  char* token;
  int frequency, spreadFactor, bandwidth;

  // Trouver la position du saut de ligne '\n'
  char* startOfValues = strchr(message, '\n');
  if (startOfValues != NULL) {
    // Passer au premier caractère des données numériques
    startOfValues++;

    // Recup après le saut de ligne
    token = strtok(startOfValues, ";");
    token = strtok(NULL, ";");

    if (token != NULL) {
      frequency = atoi(token); // Convertir en entier
      token = strtok(NULL, ";");
    }

    if (token != NULL) {
      spreadFactor = atoi(token); // Convertir en entier
      token = strtok(NULL, ";");
    }

    if (token != NULL) {
      bandwidth = atoi(token); // Convertir en entier
    }

    // Affichage des valeurs pour vérification (peut être commenté ou supprimé après vérification)
    Serial.print("Fréquence: ");
    Serial.println(frequency);
    Serial.print("Spread Factor: ");
    Serial.println(spreadFactor);
    Serial.print("Bandwidth: ");
    Serial.println(bandwidth);
  }
}

void reconnect() {
  // Boucle jusqu'à ce que la connexion soit rétablie
  while (!client.connected()) {
    u8x8.drawString(0, 2, "MQTT Conn...");
    if (client.connect("MIDI2")) {
      u8x8.drawString(0, 2, "MQTT Connected");
      client.subscribe(mqtt_topic);
    } else {
      delay(5000);
    }
  }
}
