# Mandiri e-Money Reader Lite

![PlatformIO](https://badges.pio.cc/projects/160511.svg?branch=main)
![License](https://img.shields.io/badge/license-MIT-blue.svg)
![ESP32-C3](https://img.shields.io/badge/board-ESP32--C3-green.svg)

Pembaca saldo kartu **Mandiri e-Money** menggunakan ESP32-C3 + PN532 NFC melalui SPI. Versi lite/standalone tanpa LCD, hanya output via Serial Monitor.

## ✨ Fitur

- 📖 Baca saldo Mandiri e-Money secara real-time
- 🎯 Tampilan nomor kartu dan UID
- 💰 Format saldo dengan pemisah ribuan (Rp 20.000)
- ⚡ Respons cepat dengan SPI communication
- 🔧 Minimal dependency - standalone firmware
- 📦 Ukuran firmware kecil (~35KB)

## 🛠️ Hardware

### Komponen yang Dibutuhkan

| Komponen | Kuantitas |
|----------|-----------|
| ESP32-C3 (Super Mini/DevKit) | 1 |
| PN532 NFC Module | 1 |
| Jumper wires | - |

### Wiring PN532 ke ESP32-C3 (SPI Mode)

```
PN532 NFC Module    →    ESP32-C3
-----------------------------------
SCK  (CLK)         →    GPIO 4
MISO (MISO)        →    GPIO 5
MOSI (MOSI)        →    GPIO 6
SS/CS (CS)         →    GPIO 7
VCC                →    3.3V
GND                →    GND
```

**Catatan Penting:**
- Pastikan **switch modul PN532** di posisi **SPI**
- Gunakan power supply stabil (PN532 butuh arus cukup saat aktif)
- Tambahkan kapasitor 100µF antara VCC-GND dekat modul jika diperlukan

## 🚀 Instalasi & Penggunaan

### 1. Clone Repository

```bash
git clone https://github.com/agusibrahim/emoney_reader_demo.git
cd emoney_reader_demo
```

### 2. Buka dengan PlatformIO

Project ini menggunakan **PlatformIO** (bukan Arduino IDE).

```bash
# Install dependencies
pio pkg install

# Build firmware
pio run

# Upload ke ESP32-C3
pio run -t upload

# Buka Serial Monitor
pio device monitor
```

### 3. Cara Pakai

1. Hubungkan ESP32-C3 ke computer via USB-C
2. Buka Serial Monitor (baudrate: 115200)
3. Tempelkan kartu Mandiri e-Money ke modul PN532
4. Saldo akan ditampilkan secara otomatis

**Output Serial:**
```
=================================
  Mandiri e-Money Reader Lite
=================================
[PN532] Firmware: 0x07210215 OK
Sistem siap. Tempelkan kartu Mandiri e-Money...
=================================

---------------------------------
Tipe  : Mandiri e-Money
UID   : 4F755DCA
Nomor : 0000000123456789
Saldo : Rp 50.000
=================================
```

## 📁 Struktur Project

```
emoney_reader_demo/
├── src/
│   └── main.cpp           # Main firmware
├── lib/
│   ├── PN532/              # PN532 core library
│   ├── PN532_SPI/          # SPI interface
│   └── PN532_I2C/          # I2C interface (unused)
├── platformio.ini          # Build configuration
└── README.md
```

## ⚙️ Konfigurasi

### Mengubah Pin SPI

Edit `src/main.cpp`:
```cpp
#define SPI_SCK  4   // Ubah pin SCK
#define SPI_MISO 5   // Ubah pin MISO
#define SPI_MOSI 6   // Ubah pin MOSI
#define SPI_CS   7   // Ubah pin CS
```

### Board lain

Edit `platformio.ini` dan ubah `board`:
```ini
[env:esp32c3]
board = esp32-c3-devkitc-02    ; Ganti ke board lain
```

## 🔍 Teknis

### APDU Commands

Mandiri e-Money menggunakan Java Card dengan APDU commands berikut:

```cpp
// Select Application
00 A4 04 00 08 00 00 00 00 00 00 00 00 01

// Read Card Number
00 B3 00 00 3F

// Read Balance
00 B5 00 00 0A
```

### SAK Detection

Kartu Mandiri e-Money memiliki SAK value:
- `0x20` - Mifare DESFire 4K
- `0x28` - Mifare DESFire EV1/EV2/EV3
- `0x38` - Mifare DESFire EV1

### Critical Timing

- **40ms delay** setelah `readPassiveTargetID` untuk Java Card boot-up
- **15ms delay** setelah setiap APDU exchange

## 🐛 Troubleshooting

### "PN532 tidak terdeteksi"
- Cek wiring SPI (SCK, MISO, MOSI, CS)
- Pastikan switch PN532 di posisi SPI
- Cek power supply (3.3V stabil)

### "Kartu terdeteksi tapi saldo tidak muncul"
- Pastikan kartu adalah **Mandiri e-Money** (bukan produk Mandiri lain)
- Coba tempel lebih lama/tenggelamkan ke modul
- Tambahkan kapasitor 100µF di PN532

### "Serial Monitor tidak muncul"
- Cek baudrate: **115200**
- Cek driver USB-C serial terinstall
- Coba cabut-colok USB

## 📝 Catatan

- Firmware ini **khusus untuk Mandiri e-Money**
- Kartu e-money lain (Flazz, Brizzi, TapCash) **baca keterangan dibawah**
- Untuk multi-card support, lihat keterangan dibawah

## ⚠️ Disclaimer

**Mandiri** dan **e-Money** adalah merek dagang terdaftar milik Bank Mandiri. Project ini **tidak berafiliasi** dengan, tidak didukung oleh, dan tidak disetujui oleh Bank Mandiri. Project ini adalah proyek komunitas independen untuk tujuan pembelajaran dan penggunaan personal.

## 💼 Layanan Komersial

Butuh pembaca kartu e-money yang mendukung **semua provider** (Flazz, Brizzi, TapCash, MegaCash, dll)?

Hubungi: **hello@agusibrah.im**

## 📄 License

MIT License - bebas digunakan untuk personal/commercial

## 🤝 Kontribusi

Pull request welcome! Untuk fitur request/bug report, buat issue baru.

## 👤 Author

**Agus Ibrahim**
- GitHub: [@agusibrahim](https://github.com/agusibrahim)

## 🙏 Credit

- [Adafruit PN532 Library](https://github.com/adafruit/Adafruit-PN532)
- [PrepaidCardReader Library](https://github.com/agusibrahim/esp32-irdump) - Full version dengan multi-card support

---

⭐ **Star** jika project ini membantu!
