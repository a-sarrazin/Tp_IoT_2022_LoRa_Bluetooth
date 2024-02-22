#include <WiFi.h>
#include <PubSubClient.h>
#include <LoRa.h>
#include <SPI.h>

#define SCK   5  // GPIO5  -- SX127x's SCK
#define MISO  19 // GPIO19 -- SX127x's MISO
#define MOSI  27 // GPIO27 -- SX127x's MOSI
#define CS    18 // GPIO18 -- SX127x's CS
#define RST   14 // GPIO14 -- SX127x's RESET
#define DI0   26 // GPIO26 -- SX127x's IRQ(Interrupt Request)

// LoRa settings
const double freq = 867E6;
const int sf = 9;
const double sb = 125E3;

// WiFi
const char *ssid = "raspi-webgui"; // Enter your Wi-Fi name
const char *password = "RASPI_TP";  // Enter Wi-Fi password

// MQTT Broker
const char *mqtt_broker = "10.3.141.205";
const char *topic = "srt5/midi";
const int mqtt_port = 1883;

// CSV Data
const char * csv_data = "group;freq;sf;sb\nmidi;867;9;125";

union pack {
      uint8_t frame[16]; // trames avec octets
      float  data[4];   // 4 valeurs en virgule flottante
} sdp ;  // paquet d’émission

// Function prototypes
void callback(char *topic, byte *payload, unsigned int length);

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
    // Set software serial baud to 115200;
    Serial.begin(115200);

    pinMode(DI0, INPUT);  // signal interrupt
    SPI.begin(SCK,MISO,MOSI,SS);
    LoRa.setPins(SS,RST,DI0);
    if (!LoRa.begin(freq)) {
      Serial.println("Starting LoRa failed!");
      while (1);
    }

    // Configure LoRa module
    // Frequency : 868MHz
    // 
    LoRa.setFrequency(freq);
    LoRa.setSpreadingFactor(sf);
    LoRa.setSignalBandwidth(sb);

    // Connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the Wi-Fi network");

    //connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str())) {
            Serial.println("Public MQTT broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }

    // Publish and subscribe
    // client.publish(topic, csv_data);
    // client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

void loop() {
  client.publish(topic, csv_data);
  delay(5000);
  // client.loop();
  // Serial.println("New Packet") ;
  // LoRa.beginPacket();
  // sdp.data[0]=d1;
  // sdp.data[1]=d2;
  // LoRa.write(sdp.frame,16);
  // LoRa.endPacket();
  // d1++; d2+=2;
  // delay(2000);
}