#include "WiFi.h"
#include <U8x8lib.h>
#include <PubSubClient.h>
#include <LoRa.h>
#include <SPI.h>


#define SCK   5  // GPIO5  -- SX127x's SCK
#define MISO  19 // GPIO19 -- SX127x's MISO
#define MOSI  27 // GPIO27 -- SX127x's MOSI
#define CS    18 // GPIO18 -- SX127x's CS
#define RST   14 // GPIO14 -- SX127x's RESET
#define DI0   26 // GPIO26 -- SX127x's IRQ(Interrupt Request)


// Paramètres de connexion WiFi
const char* ssid = "raspi-webgui";
const char* password = "RASPI_TP";

// Paramètres de connexion MQTT
const char* mqtt_server = "10.3.141.205";
const int mqtt_port = 1883;
const char* mqtt_topic = "srt5/midi";

// Typage structure données sur LoRa
typedef struct tcp_socket_info {
  uint8_t ipv4[4]; // 32bits => IP
  uint16_t port;   // 16bits => port
  char mdp[8];     // 64bits => mot de passe
} tcp_socket_info_t;

// Empaquetage
union pack {
      uint8_t frame[sizeof(tcp_socket_info_t)]; // trames avec octets
      tcp_socket_info_t tcp_socket;
} rdp ;  // paquet d’envoi

// Initialisation du message avec des valeurs d'exemple
const tcp_socket_info_t MSG = {
  {WiFi.localIP().toString().c_str()}, // Adresse IP exemple: 192.168.1.1
  8080,             // Port exemple: 8080
  "password"        // Mot de passe exemple: "password" (à modifier pour respecter votre politique de sécurité)
};

// Initialisation l'écran OLED
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(15, 4, 16);

// Client WiFi pour la connexion au réseau
WiFiClient espClient;
// Client MQTT
PubSubClient client(espClient);

unsigned long lastMsgTime = 0; // Temps auquel le dernier message a été envoyé
const long interval = 100000;   // Intervalle de temps entre les messages (10 secondes)
int frequency, spreadFactor, bandwidth;


union pack
{
  uint8_t frame[16]: // trames avec octets
  float data[4]; // 4 valeurs en virgule flottante
} sdp; // paquet d'émission












void setup() {
  // Initialiser le port série pour le débogage
  Serial.begin(115200);

  pinMode(DI0, INPUT);  // signal interrupt
    SPI.begin(SCK,MISO,MOSI,SS);
    LoRa.setPins(SS,RST,DI0);
    if (!LoRa.begin(frequency)) {
      Serial.println("Erreur LoRa");
      while (1);
    }

  // Configure LoRa module
  // Frequency : 868MHz
  // 

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
    
  LoRa.setFrequency(frequency);
  LoRa.setSpreadingFactor(spreadFactor);
  LoRa.setSignalBandwidth(bandwidth);

  sendMessage();    // Envoi du message
  }
}












void sendMessage() {
  Serial.printin ("ENVOI !") ;
  LoRa.beginPacket() ;
  sdp.data[0]="Bonjour";
  sdp.data[1]="";
  LoRa.write (sdp.frame, 16) ;
  LoRa.endPacket() ; // start packet
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

    // Récupération et affichage de l'adresse IP
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Server Port: ");
    Serial.println(server.port());
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
