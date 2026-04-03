#include <Arduino.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>

// Konfigurasi Pin SPI ESP32-C3
#define SPI_SCK  4
#define SPI_MISO 5
#define SPI_MOSI 6
#define SPI_CS   7

PN532_SPI pn532spi(SPI, SPI_CS);
PN532 nfc(pn532spi);

// Mandiri APDU Commands
const uint8_t APDU_SELECT_APP[]   = {0x00, 0xA4, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
const uint8_t APDU_READ_CARD_INFO[] = {0x00, 0xB3, 0x00, 0x00, 0x3F};
const uint8_t APDU_GET_BALANCE[]  = {0x00, 0xB5, 0x00, 0x00, 0x0A};

char hexBuf[256];
const char* bytesToHex(const uint8_t* data, size_t len) {
    for (size_t i = 0; i < len && i < 127; i++) {
        sprintf(hexBuf + i * 2, "%02X", data[i]);
    }
    hexBuf[len * 2] = 0;
    return hexBuf;
}

bool isSuccess(const uint8_t* response, size_t len) {
    if (len < 2) return false;
    uint8_t sw1 = response[len - 2];
    uint8_t sw2 = response[len - 1];
    return (sw1 == 0x90 || sw1 == 0x91) && sw2 == 0x00;
}

String formatNumber(long num) {
    String str = String(num);
    String result = "";
    int len = str.length();
    int count = 0;
    for (int i = len - 1; i >= 0; i--) {
        if (count > 0 && count % 3 == 0) {
            result = "." + result;
        }
        result = str[i] + result;
        count++;
    }
    return result;
}

bool sendAPDU(const uint8_t* cmd, size_t cmdLen, uint8_t* response, uint8_t* responseLen) {
    if (!nfc.inDataExchange((uint8_t*)cmd, cmdLen, response, responseLen)) {
        return false;
    }
    delay(15);
    return true;
}

bool readMandiri(const String& tagId) {
    uint8_t res[250];
    uint8_t resLen;
    
    // 1. Select App
    resLen = sizeof(res);
    if (!sendAPDU(APDU_SELECT_APP, sizeof(APDU_SELECT_APP), res, &resLen)) return false;
    if (!isSuccess(res, resLen)) return false;
    
    // 2. Read File (Number)
    resLen = sizeof(res);
    if (!sendAPDU(APDU_READ_CARD_INFO, sizeof(APDU_READ_CARD_INFO), res, &resLen)) return false;
    if (!isSuccess(res, resLen)) return false;
    
    String cardNumber = tagId;
    if (resLen >= 8) {
        cardNumber = String(bytesToHex(res, 8));
    }
    
    // 3. Get Balance
    resLen = sizeof(res);
    if (!sendAPDU(APDU_GET_BALANCE, sizeof(APDU_GET_BALANCE), res, &resLen)) return false;
    if (resLen < 4) return false;
    
    long balance = 0;
    for (int i = 0; i < 4; i++) {
        balance += ((long)res[i]) << (i * 8);
    }
    
    Serial.println("---------------------------------");
    Serial.println("Tipe  : Mandiri e-Money");
    Serial.printf("UID   : %s\n", tagId.c_str());
    Serial.printf("Nomor : %s\n", cardNumber.c_str());
    Serial.printf("Saldo : Rp %s\n", formatNumber(balance).c_str());
    Serial.println("=================================\n");
    return true;
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=================================");
    Serial.println("  Mandiri e-Money Reader Lite");
    Serial.println("=================================");

    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, -1);
    nfc.begin();

    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
        Serial.println("[ERROR] PN532 tidak terdeteksi via SPI!");
        while (1) { delay(10); }
    }

    Serial.printf("[PN532] Firmware: 0x%08X OK\n", versiondata);
    nfc.SAMConfig();
    nfc.setPassiveActivationRetries(0x10);

    Serial.println("Sistem siap. Tempelkan kartu Mandiri e-Money...");
    Serial.println("=================================\n");
}

void loop() {
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
    uint8_t uidLength;
    
    bool success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 250);
    
    if (success) {
        delay(40); // Boot-up delay untuk Java Card
        
        String tagId = "";
        for (uint8_t i = 0; i < uidLength; i++) {
            char h[3];
            sprintf(h, "%02X", uid[i]);
            tagId += h;
        }
        
        uint8_t buffLen;
        uint8_t sak = nfc.getBuffer(&buffLen)[4];
        
        // Pengecekan ISODEP (SAK 0x20/0x28)
        if (sak == 0x20 || sak == 0x28 || sak == 0x38) {
            if (readMandiri(tagId)) {
                delay(2000); // Tahan output agar tidak spam jika sukses
                return;
            } else {
                // Serial.println("[INFO] Kartu terdeteksi tapi bukan Mandiri e-Money atau bacaan gagal.");
                delay(500); // Jeda retry cepat jika sekadar error APDU
            }
        } else {
            Serial.printf("[INFO] Kartu terdeteksi (UID: %s, SAK: 0x%02X) - Bukan Mandiri e-Money.\n", tagId.c_str(), sak);
            delay(1000);
        }
        
        // Bersihkan state RF supaya PN532 siap membaca kartu lagi
        nfc.inRelease();
        nfc.setRFField(0, 0);
    }
}
