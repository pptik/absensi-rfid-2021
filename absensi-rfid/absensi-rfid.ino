
#include <FS.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include "MFRC522.h"
#include <PubSubClient.h>
#include "config.h"

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED1, OUTPUT);
  digitalWrite(BUZZER, LOW);
  Serial.begin(115200);
  Serial.println(F("Booting...."));
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  delay(100);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println(F("Ready!"));
  Serial.println(F("======================================================"));
  Serial.println(F("Scan for Card and print UID:"));
}

void setup_wifi() {
  WiFi.macAddress(MAC_array);
  for (int i = 0; i < sizeof(MAC_array) - 1; ++i) {
    sprintf(MAC_char, "%s%02x:", MAC_char, MAC_array[i]);
  }
  sprintf(MAC_char, "%s%02x", MAC_char, MAC_array[sizeof(MAC_array) - 1]);
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  String(mqtt_port).toCharArray(smqtt_port, 5);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", smqtt_port, 5);
  WiFiManagerParameter custom_mqtt_user("user", "mqtt user", mqtt_user, 40);
  WiFiManagerParameter custom_mqtt_password("password", "mqtt password", mqtt_password, 40);
  WiFiManagerParameter custom_mqtt_keywords1("keyword1", "mqtt keyword1", mqtt_keywords1, 40);
  WiFiManagerParameter custom_mqtt_keywords2("keyword2", "mqtt keyword2", mqtt_keywords2, 40);
  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_password);
  wifiManager.addParameter(&custom_mqtt_keywords1);
  wifiManager.addParameter(&custom_mqtt_keywords2);
  if (!wifiManager.autoConnect("AbesensiRFID", "password")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);

    ESP.reset();
    delay(5000);
  }
  Serial.println("");
  Serial.println("WiFi connected To:");
  Serial.println(WiFi.SSID());
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  String sTopic = topic;

  if (sTopic == mqtt_keywords2)
  {
    Serial.println("Sense Control Command");
    if ((char)payload[1] == '1')
    {
      ResetSettings();
    }

    if ((char)payload[0] == '1')
    {
      delay(2000);
      ESP.reset();
      delay(5000);
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    digitalWrite(LED1, LOW);
    digitalWrite(BUZZER, HIGH);
    delay(500);
    digitalWrite(BUZZER, LOW);
    delay(1500);
    Serial.println(mqtt_server);
    if (client.connect(MAC_char, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      digitalWrite(LED1, HIGH);
      client.publish(mqtt_keywords1, MAC_char);
      client.subscribe(mqtt_keywords2);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      if (client.state() == 4) ESP.restart();
      else {
        Serial.println(" try again in 5 seconds");
        delay(5000);
      }
    }
  }
}

unsigned long previousMillis = 0;
bool delayMeas = false;
int value = 0;

void loop() {
  digitalWrite(BUZZER, LOW);
  unsigned long currentMillis = millis();
  if (!client.connected()) {
    digitalWrite(BUZZER, LOW);
    reconnect();
  }
  client.loop();
  if (delayMeas)
  {
    if ((currentMillis - previousMillis) >= interval)
    {
      previousMillis = currentMillis;
      delayMeas = false;
      delay(1800);

    }
  }
  else
  {
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      digitalWrite(BUZZER, LOW);
      delay(550);
      return;
    }
    digitalWrite(BUZZER, HIGH);
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      digitalWrite(BUZZER, LOW);
      delay(900);
      return;
    }
    char msg[150];
    char msg1[150];
    // Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    msg[0] = char(0);
    msg1[0] = char(0);
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      sprintf(msg, "%s0x%02x ", msg, mfrc522.uid.uidByte[i]);
    }
    sprintf(msg1, "{\"mac\":\"%s\",\"rf_id\":\"%s\"}", MAC_char, msg);
    Serial.println();
    Serial.print("Publish message: ");
    Serial.println(msg1);
    client.publish(mqtt_keywords1, msg1);
    msg[0] = char(0);
    delayMeas = true;
    digitalWrite(BUZZER, LOW);
    digitalWrite(LED1, HIGH);
    delay(220);
    digitalWrite(LED1, LOW);
    delay(100);
    digitalWrite(LED1, HIGH);
    delay(200);
    digitalWrite(LED1, LOW);
    delay(100);
    digitalWrite(LED1, HIGH);
    delay(100);
    digitalWrite(LED1, LOW);
    delay(100);
    digitalWrite(LED1, HIGH);
  }
}
