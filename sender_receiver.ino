#include "WiFi.h"
#include <U8x8lib.h>
#include <PubSubClient.h>

// Paramètres de connexion WiFi
const char* ssid = "Iphone Giovanni";
const char* password = "Giovanni85*";

// Paramètres de connexion MQTT
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "srt5/alban";
const char* message_text = "MIDI2 Bonjour les SRT5 !"; // Message envoyé

// Initialisation l'écran OLED
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(15, 4, 16);

// Client WiFi pour la connexion au réseau
WiFiClient espClient;
// Client MQTT
PubSubClient client(espClient);

unsigned long lastMsgTime = 0; // Temps auquel le dernier message a été envoyé
const long interval = 10000;   // Intervalle de temps entre les messages (10 secondes)

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
    u8x8.drawString(0, 0, "Connecting...");
  }

  // Affichage de l'adresse IP une fois connecté
  u8x8.drawString(0, 0, "WiFi Connected");
  u8x8.drawString(0, 1, WiFi.localIP().toString().c_str());

  // Configuration des paramètres du serveur MQTT et la fonction de callback
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Connexion au serveur MQTT
  if (client.connect("MIDI2")) {
    u8x8.drawString(0, 2, "MQTT Connected");
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
    
    // Envoi du message
    sendMessage();
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
