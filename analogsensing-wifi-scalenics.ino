/*
  Analog Sensing for ESPr One(ESP-WROOM-02)

  This sketch connects to a IoT cloud "Scalenics" (https://api.scalenics.io/console/)
  using ESP-WROOM-02 (Wi-Fi).

  The circuit:
  *Input PIN
    A0:Analog in
  *Output PIN
    None

  Created 13 July 2016
  by LoonaiFactory

  https://github.com/simics-co/analogsensing-wifi-scalenics
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Connection info data lengths
#define MAC_ADDR_LEN    6   // Length of MAC address in bytes

// Constants
unsigned char mac_addr[MAC_ADDR_LEN];

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 1, 0);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// WiFi
const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Scalenics
#define SC_USER "YOUR_SCALENICS_ACCOUNT"
#define DEVICE_TOKEN "YOUR_DEVICE_TOKEN_HERE"
#define CLIENT_ID "enocean"
#define MQTT_SERVER "api.scalenics.io"

const uint32_t deviceID = 0x01234567;

WiFiClient client;

// MQTT client
PubSubClient mqttClient(MQTT_SERVER, 1883, NULL, client);

static void connect(void);

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  connect();
}

#define TIME_DELAY 1000  /* msec */
String topic;
String postData;  
char mqtt_topic[64];
char mqtt_payload[32];

const int sensorPin = A0;
char postID[4];
char state[5];

void loop()
{
  delay(TIME_DELAY);
  
  int sensorValue = analogRead(sensorPin);
  
  topic = DEVICE_TOKEN;
  topic += "/";
  sprintf(postID, "%08lX", deviceID);
  topic += postID;
  topic.toCharArray(mqtt_topic, topic.length() + 1);
  
  postData = "";
  postData += "v=";
  sprintf(state, "%d", sensorValue);
  postData += state;
  postData.toCharArray(mqtt_payload, postData.length() + 1);
  
  Serial.print(F("  mqtt_topic : "));
  Serial.println(mqtt_topic);
  Serial.print(F("  mqtt_payload : "));
  Serial.println(mqtt_payload);
  
  if (mqttClient.connect(postID, SC_USER, DEVICE_TOKEN)) {
    Serial.println(F("  Connection to MQTT server succeeded"));
    mqttClient.publish(mqtt_topic, mqtt_payload);
  } else {
    int state = mqttClient.state();
    Serial.print(F("  Connection to MQTT server failed: "));
    Serial.println(state);
  }
}

static void connect(void)
{
  Serial.println();
  
  // give the wifi module time to boot up:
  delay(100);
  
  // read and display MAC address
  String mac = WiFi.macAddress();
  Serial.print(F("MAC Address: "));
  Serial.println(mac);
  
  // start the Ethernet connection:
  Serial.print(F("Connecting to "));
  Serial.print(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  uint8_t timeout = 60; // 60 * 500 msec = 30 sec time out
  while ((WiFi.status() != WL_CONNECTED) && timeout) {
    delay(500);
    Serial.print(F("."));
    timeout--;
  }
  Serial.println();
  
  if(WiFi.status() != WL_CONNECTED) {
    Serial.print(F("Setting static IP"));
    WiFi.config(ip, gateway, subnet);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(F("."));
    }
    Serial.println();
  }
  
  Serial.println(F("WiFi connected"));
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());
}
