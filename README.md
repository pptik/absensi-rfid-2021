# Absensi-rfid-2021
## Cara setting Microcontroller Absensi RFID

### Software yang diperlukan : 
- Arduino IDE (Terbaru).

### Library Arduino IDE yang diperlukan:
- ESP versi 2.4.1.
- Dan Library yang ada di [Download Library]( https://github.com/pptik/absensi-rfid-2021/tree/main/Library).

#### Hardware yang digunakan: 
- NodeMCU Amica ESP8288 / NodeMCU Lolin ESP8266.
- Micro USB Data.
- RFID-RC522

### 1. Untuk menginstall ikuti langkah berikut: 
- Buka Arduino IDE.
- Sambungkan Node MCU dan Kabel USB ke Port USB di port USB .
- Buka file absensi-rfid.ino [File](https://github.com/pptik/absensi-rfid-2021/blob/main/absensi-rfid/ConfigFile.ino), Pastikan dalam satu folder ada file [Configfile.ino](https://github.com/pptik/absensi-rfid-2021/blob/main/absensi-rfid/ConfigFile.ino) dan [Config.h](https://github.com/pptik/absensi-rfid-2021/blob/main/absensi-rfid/config.h).
- Import Library yang telah disiapkan  atau download di [Download Library]( https://github.com/pptik/absensi-rfid-2021/tree/main/Library).
- Pilih Board yang akan di install di Arduino IDE
    > Tools -> Board -> Esp8266 Modules -> NodeMCU 1.0 (ESP-12E Module).
- Pilih upload speed di Arduino IDE
    > Tools -> Upload Speed -> 115200.
- Tekan ***Ctrl+U*** untuk upload.
- Tunggu hingga Done uploading.


### 2. Mengkoneksikan microcontroller ke Wifi
- Jika semua komponen telah dirakit sambungkan Microcontroller ke sumber listrik.
- Di komputer atau smartphone 
    > Buka wifi -> masuk ke wifi AbsensiRFID -> masukan password -> passwordnya (password).
- Setelah itu
    > Buka browser -> Input ip 192.168.4.1 -> Tekan Configure Wifi -> Pilih SSID Wifi yang akan digunakan -> masukan password wifi -> Save.
- Jika sudah bersuara beep wifi sudah terhubung.
- Jika suara beep bersuara terus menerus wifi terhubung namun tidak ada koneksi internet. **Jadi pastikan _wifi terhubung dengan internet_**.
- Tempelkan kartu RFID jika ada suara beep 2x berarti data sudah berhasil dikirimkan ke server.
