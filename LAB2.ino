/* Project: default */
#include <WiFi.h>

#include "PubSubClient.h"


const char espwifi_ssid[] = "JOE";
const char espwifi_pass[] = "Gorditofeliz212";

void espwifi_setup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(espwifi_ssid, espwifi_pass);
  Serial.println("Conectando a la red Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("");
  Serial.println("conexión realizada con éxito");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

bool conectado;
unsigned long impr_conect_mqtt = 0;
const char espmqtt_broker[] = "mqtt3.thingspeak.com";
const int espmqtt_port = 1883;
const char espmqtt_user[] = "HTUtMTINITozLCYCAjs0HCM";
const char espmqtt_pass[] = "jxG+b3ZO3D/Q0wRZrIaTDyEY";
const char espmqtt_clientid[] = "HTUtMTINITozLCYCAjs0HCM";
WiFiClient espmqtt_wifiClient;
PubSubClient espmqtt_client(espmqtt_wifiClient);
String espmqtt_topic = "";
String espmqtt_msg_str = "";
double espmqtt_msg_number = 0;
char espmqtt_payload[128];

void espmqtt_onreceive(){};
void espmqtt_setup() {
  delay(10);
  randomSeed(micros());
  espmqtt_client.setServer(espmqtt_broker, espmqtt_port);
  espmqtt_client.setCallback(espmqtt_callback);
  espmqtt_subscribe();
}
void espmqtt_loop() {
  if (!espmqtt_client.connected()) {
    espmqtt_client.connect(espmqtt_clientid, espmqtt_user, espmqtt_pass);
    if ((millis() - impr_conect_mqtt) >= 1000) {
      Serial.println("Conectando al servidor mqtt");
      impr_conect_mqtt = millis();
    }
    conectado = false;
    espmqtt_subscribe();
  }
  if (espmqtt_client.connected()) {
    if (!conectado) {
      Serial.println("Conectado con éxito");
      conectado = true;
    }
    espmqtt_client.loop();
  }
}
double espmqtt_payload2double(unsigned char* _payload, int _length) {
  int i;
  for (i = 0; i < _length && i < 128; i++) {
    espmqtt_payload[i] = _payload[i];
  }
  espmqtt_payload[i] = 0;
  return atof(espmqtt_payload);
}
String espmqtt_payload2string(unsigned char* _payload, int _length) {
  int i;
  for (i = 0; i < _length && i < 128; i++) {
    espmqtt_payload[i] = _payload[i];
  }
  espmqtt_payload[i] = 0;
  return String(espmqtt_payload);
}

#include "DHT.h";
DHT dht32(32, DHT11);

unsigned long frecuenciaPubl_ = 0;


void espmqtt_subscribe() {
}

void espmqtt_callback(char* _topic, unsigned char* _payload, unsigned int _payloadlength) {
  espmqtt_topic = String(_topic);
  espmqtt_msg_str = espmqtt_payload2string(_payload, _payloadlength);
  espmqtt_msg_number = espmqtt_payload2double(_payload, _payloadlength);

  espmqtt_onreceive();
}

void setup() {
  Serial.begin(115200);
  espwifi_setup();
  espmqtt_setup();
  pinMode(32, INPUT);
  dht32.begin();
}

void loop() {
  yield();
  espmqtt_loop();
  if (millis() - frecuenciaPubl_ > 23000) {
    frecuenciaPubl_ = millis();
    espmqtt_client.publish("channels/2557038/publish/fields/field1", String(dht32.readTemperature()).c_str());
  }
}