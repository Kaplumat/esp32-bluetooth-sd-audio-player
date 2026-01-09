#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <BluetoothA2DPSource.h>
#include <FS.h>

// SD kart pinleri
#define SD_CS 5
#define SD_MOSI 23
#define SD_MISO 19
#define SD_SCK 18

BluetoothA2DPSource a2dp_source;

// Kulaklık MAC adresi
static const char* TARGET_BD_ADDR = "34:D6:93:54:CD:56";
esp_bd_addr_t target_addr;

// Ses seviyesi
#define VOLUME 0.2  // 0.0 - 1.0 arası

// Double buffer
#define BUFFER_SIZE 24576  // 16 KB per buffer
uint8_t bufferA[BUFFER_SIZE];
uint8_t bufferB[BUFFER_SIZE];

uint8_t* currentBuffer = bufferA;
uint8_t* fillBuffer = bufferB;
size_t buffer_len = 0;
size_t buffer_pos = 0;
bool bufferReady = false;

File audio_file;

// MAC adresi string -> esp_bd_addr_t
void string_to_bd_addr(const char* addr_str, esp_bd_addr_t bd_addr) {
    int values[6];
    sscanf(addr_str, "%x:%x:%x:%x:%x:%x", 
           &values[0], &values[1], &values[2], 
           &values[3], &values[4], &values[5]);
    for (int i = 0; i < 6; i++) {
        bd_addr[i] = (uint8_t)values[i];
    }
}

// Bluetooth bağlantı durumu
void connection_state_changed(esp_a2d_connection_state_t state, void* ptr) {
    Serial.print("Connection state changed: ");
    switch (state) {
        case ESP_A2D_CONNECTION_STATE_DISCONNECTED: Serial.println("Disconnected"); break;
        case ESP_A2D_CONNECTION_STATE_CONNECTING:   Serial.println("Connecting"); break;
        case ESP_A2D_CONNECTION_STATE_CONNECTED:    Serial.println("Connected"); break;
        case ESP_A2D_CONNECTION_STATE_DISCONNECTING:Serial.println("Disconnecting"); break;
    }
}

// A2DP callback: PCM frame doldur
int32_t a2dp_data_callback(Frame* frame, int32_t len) {
    for (int32_t i = 0; i < len; i++) {
        if (buffer_pos + 4 <= buffer_len) {
            int16_t left = ((int16_t)(currentBuffer[buffer_pos] | (currentBuffer[buffer_pos+1]<<8)) * VOLUME);
            int16_t right = ((int16_t)(currentBuffer[buffer_pos+2] | (currentBuffer[buffer_pos+3]<<8)) * VOLUME);
            frame[i].channel1 = left;
            frame[i].channel2 = right;
            buffer_pos += 4;
        } else {
            // Buffer bitti → swap yap
            if (bufferReady) {
                uint8_t* temp = currentBuffer;
                currentBuffer = fillBuffer;
                fillBuffer = temp;
                buffer_pos = 0;
                buffer_len = BUFFER_SIZE;
                bufferReady = false;
            } else {
                frame[i].channel1 = 0;
                frame[i].channel2 = 0;
            }
        }
    }
    return len;
}

// SD’den buffer doldur
void fill_sd_buffer() {
    if (!bufferReady) {
        buffer_len = audio_file.read(fillBuffer, BUFFER_SIZE);
        if (buffer_len == 0) {
            audio_file.seek(44); // WAV header atla
            buffer_len = audio_file.read(fillBuffer, BUFFER_SIZE);
        }
        bufferReady = true;
    }
}

bool load_wav_file(const char* filename) {
    if (audio_file) audio_file.close();
    audio_file = SD.open(filename);
    if (!audio_file) {
        Serial.print("Failed to open file: ");
        Serial.println(filename);
        return false;
    }
    audio_file.seek(44); // WAV header atla
    buffer_len = audio_file.read(currentBuffer, BUFFER_SIZE);
    buffer_pos = 0;
    bufferReady = false; // fillBuffer henüz doldurulmadı
    Serial.print("Loaded ");
    Serial.print(buffer_len);
    Serial.println(" bytes into buffer");
    return true;
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    // SD kart
    Serial.println("Initializing SD card...");
    SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS)) {
        Serial.println("SD init failed!");
        while (1) delay(1000);
    }
    Serial.println("SD initialized.");

    // Küçük delay ekleyelim → ESP32 heap ve A2DP stack stabil olsun
    delay(2000);

    // Bluetooth
    Serial.println("Initializing Bluetooth...");
    a2dp_source.set_on_connection_state_changed(connection_state_changed);
    a2dp_source.set_data_callback_in_frames(a2dp_data_callback);

    string_to_bd_addr(TARGET_BD_ADDR, target_addr);
    a2dp_source.set_auto_reconnect(target_addr);

    a2dp_source.start("ESP32-AudioPlayer");
    Serial.println("Bluetooth started. Connecting to target device...");

    // WAV dosyasını yükle
    if (!load_wav_file("/Car.wav")) {
        Serial.println("Cannot load WAV file!");
        while (1) delay(1000);
    }
}

void loop() {
    fill_sd_buffer();
    yield(); // FreeRTOS’a CPU bırak → resetleri engeller
}
