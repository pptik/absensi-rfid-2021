/*
    PLEASE DONT DELETE AND EDIT
    Develop by          : Nurman Hariyanto
    Email               : nurman.hariyanto13@gmail.com
    Project             : Absensi RFID 
    Version             : 1.0
    Description         : This code for get data from RFID card and send data to server
    Microcontroller     : Wemos Mini ESP8266
                          NodeMCU ESP8266
    Device              : MFRC522
*/


///YOU CAN EDIT NOW

/*
   Include library
*/
#include <FS.h>                   
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include "MFRC522.h"
#include <PubSubClient.h>
#include "config.h"

void setup() {
  //ResetSettings();
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(BUZZER, OUTPUT);
  pinMode(LED1, OUTPUT);
  digitalWrite(BUZZER, LOW);
  Serial.begin(115200);


  Serial.println(F("Booting...."));

  //ReadConfigFile(); // Read Config File if it is exist
  setup_wifi();
  //SaveConfigFile();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  delay(100);
  SPI.begin();
  // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522

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

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect(MAC_char, "password")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  //WiFi.begin(wifi_ssid, wifi_password);

  /* while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
    }*/

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
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    digitalWrite(LED1, LOW);
    digitalWrite(BUZZER, HIGH);
    delay(500);
    digitalWrite(BUZZER, LOW);
    delay(1500);
    Serial.println(mqtt_server);
    // Attempt to connect
    if (client.connect(MAC_char, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      digitalWrite(LED1, HIGH);
      // Once connected, publish an announcement...
      client.publish(mqtt_keywords1, MAC_char);
      client.subscribe(mqtt_keywords2);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      if (client.state() == 4) ESP.restart();
      else {
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
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

    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      digitalWrite(BUZZER, LOW);
      delay(550);
      return;
    }
    digitalWrite(BUZZER, HIGH);

    // Select one of the cards
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
      //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
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
