#include <Arduino.h>

#include <cstdio>

#include "Display/DisplayManager.h"

namespace {

constexpr uint16_t kBackground = LGFX::color565(12, 14, 18);
constexpr uint16_t kPanel = LGFX::color565(32, 38, 50);
constexpr uint16_t kAccent = LGFX::color565(0, 180, 160);
constexpr uint16_t kText = LGFX::color565(235, 240, 245);
constexpr uint16_t kMuted = LGFX::color565(90, 100, 115);

DisplayManager g_display;

void drawStaticPattern(LGFX& gfx) {
    gfx.fillScreen(kBackground);

    gfx.fillRect(12, 60, 140, 90, kPanel);
    gfx.drawRect(12, 60, 140, 90, kAccent);

    gfx.fillCircle(238, 105, 42, kAccent);
    gfx.drawLine(12, 168, 308, 168, kMuted);
    gfx.fillTriangle(190, 176, 218, 220, 162, 220, kPanel);

    gfx.setTextDatum(lgfx::textdatum_t::top_left);
    gfx.setTextColor(kText, kBackground);

    gfx.setFont(&fonts::Font4);
    gfx.drawString("ESP32S3", 12, 12);

    gfx.setFont(&fonts::Font2);
    char info[48];
    std::snprintf(info, sizeof(info), "%dx%d  SPI %lu MHz",
                  static_cast<int>(gfx.width()),
                  static_cast<int>(gfx.height()),
                  static_cast<unsigned long>(Config::DISPLAY_SPI_FREQ_HZ / 1000000UL));
    gfx.drawString(info, 12, 38);
}

void drawSpriteTest(LGFX& gfx) {
    constexpr int32_t kSpriteWidth = 220;
    constexpr int32_t kSpriteHeight = 80;

    lgfx::LGFX_Sprite sprite(&gfx);
    sprite.setColorDepth(16);

    // Preferir PSRAM; si no está disponible, usar memoria interna.
    sprite.setPsram(true);
    if (!sprite.createSprite(kSpriteWidth, kSpriteHeight)) {
        sprite.setPsram(false);
        if (!sprite.createSprite(kSpriteWidth, kSpriteHeight)) {
            Serial.println("Display: no hay memoria para el sprite");
            return;
        }
    }

    sprite.fillRect(0, 0, kSpriteWidth, kSpriteHeight, kPanel);
    sprite.drawRect(0, 0, kSpriteWidth, kSpriteHeight, kAccent);

    sprite.setTextDatum(lgfx::textdatum_t::middle_center);
    sprite.setTextColor(kText, kPanel);
    sprite.setFont(&fonts::Font4);
    sprite.drawString("SPRITE OK", kSpriteWidth / 2, kSpriteHeight / 2);

    sprite.pushSprite((gfx.width() - kSpriteWidth) / 2,
                      (gfx.height() - kSpriteHeight) / 2);
    gfx.waitDisplay();
    sprite.deleteSprite();
}

}  // namespace

void setup() {
    Serial.begin(115200);

    if (!g_display.begin()) {
        Serial.println("Display: fallo de inicialización");
        return;
    }

    LGFX& gfx = g_display.getDisplay();
    drawStaticPattern(gfx);
    drawSpriteTest(gfx);

    Serial.printf("Display listo: %dx%d\n",
                  static_cast<int>(gfx.width()),
                  static_cast<int>(gfx.height()));
}

void loop() {
    delay(1);
}
