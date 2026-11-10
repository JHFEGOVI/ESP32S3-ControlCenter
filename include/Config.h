#pragma once

#include <cstdint>

namespace Config {

// Valor centinela para pines no conectados o no definidos en el hardware.
constexpr int8_t PIN_UNUSED = -1;

// -----------------------------------------------------------------------------
// Pantalla: ILI9341V por SPI
// -----------------------------------------------------------------------------
constexpr int8_t DISPLAY_SCLK = 12;
constexpr int8_t DISPLAY_MOSI = 11;
constexpr int8_t DISPLAY_MISO = 13;
constexpr int8_t DISPLAY_CS = 10;
constexpr int8_t DISPLAY_DC = 46;
constexpr int8_t DISPLAY_RST = PIN_UNUSED;
constexpr int8_t DISPLAY_BACKLIGHT = 45;

constexpr uint32_t DISPLAY_SPI_FREQ_HZ = 40000000;

// Dimensiones nativas del panel, en retrato y antes de aplicar rotación.
constexpr uint16_t DISPLAY_NATIVE_WIDTH = 240;
constexpr uint16_t DISPLAY_NATIVE_HEIGHT = 320;

// Dimensiones lógicas en orientación apaisada (rotación 1).
constexpr uint16_t DISPLAY_WIDTH = DISPLAY_NATIVE_HEIGHT;
constexpr uint16_t DISPLAY_HEIGHT = DISPLAY_NATIVE_WIDTH;

// Brillo inicial al arrancar.
constexpr uint8_t DISPLAY_DEFAULT_BRIGHTNESS = 128;

// -----------------------------------------------------------------------------
// Táctil: FT6336G por I2C
// -----------------------------------------------------------------------------
constexpr int8_t TOUCH_SDA = 16;
constexpr int8_t TOUCH_SCL = 15;
constexpr int8_t TOUCH_RST = PIN_UNUSED;
constexpr int8_t TOUCH_INT = PIN_UNUSED;

constexpr uint8_t TOUCH_I2C_ADDR = 0x38;
constexpr uint32_t TOUCH_I2C_FREQ_HZ = 400000;

// -----------------------------------------------------------------------------
// Micro SD: SDIO 4-bit nativo (SD_MMC), no comparte bus con la pantalla
// -----------------------------------------------------------------------------
constexpr int8_t SD_CLK = 38;
constexpr int8_t SD_CMD = 40;
constexpr int8_t SD_D0 = 39;
constexpr int8_t SD_D1 = 41;
constexpr int8_t SD_D2 = 48;
constexpr int8_t SD_D3 = 47;

// -----------------------------------------------------------------------------
// Buzzer externo opcional
// -----------------------------------------------------------------------------
// Buzzer externo opcional en GPIO libre (IO2, IO3, IO14 o IO21). La placa no
// incluye buzzer; el audio integrado va por el códec ES8311 (ver bloque Audio).
constexpr int8_t BUZZER_PIN = PIN_UNUSED;

// -----------------------------------------------------------------------------
// Audio: códec ES8311 + amplificador FM8002E (altavoz externo, opcional)
// -----------------------------------------------------------------------------
constexpr int8_t AUDIO_MCLK = 4;
constexpr int8_t AUDIO_BCLK = 5;
constexpr int8_t AUDIO_LRCK = 7;
constexpr int8_t AUDIO_DOUT = 8;  // ESP32 -> códec
constexpr int8_t AUDIO_DIN = 6;   // códec -> ESP32, micrófono
constexpr int8_t AUDIO_EN = 1;    // Enable del amplificador, activo en bajo

}  // namespace Config
