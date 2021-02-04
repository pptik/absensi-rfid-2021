
#ifndef __CONFIGFILE__
#define __CONFIGFILE__
bool shouldSaveConfig = true;
// PPTIK Setting
char wifi_ssid[34] = "MIC - Gigabit";
char wifi_password[34] = "micanjas";
char mqtt_server[40] = "167.205.7.226";
int   mqtt_port = 1883;
char smqtt_port[5] = "1883";
char mqtt_user[40] = "/absensi:absensi";
char mqtt_password[40] = "PPTIK@|PASSWORD";
char mqtt_keywords1[40] = "absensi/service";
char mqtt_keywords2[40] = "absensi/control";
const long interval = 2000;
uint8_t MAC_array[6];
char MAC_char[18];  
WiFiClient espClient;
PubSubClient client(espClient);                   
#define RST_PIN  5  // RST-PIN für RC522 - RFID - SPI - Modul GPIO5 
#define SS_PIN  4  // SDA-PIN für RC522 - RFID - SPI - Modul GPIO4 
#define BUZZER  D3 // pcb merah (D3) (kalau hijau, biru D8) 
#define LED1 D8 // pcb merah (kalau hijau,biru d4)
#define LED2 D4 
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
void SaveConfigCallback ();
void ReadConfigFile();
#endif
