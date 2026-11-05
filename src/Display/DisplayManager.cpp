#include "DisplayManager.h"

bool DisplayManager::begin() {
    if (_initialized) {
        return true;
    }

    if (!_display.init()) {
        return false;
    }

    _display.setRotation(1);
    _display.setBrightness(Config::DISPLAY_DEFAULT_BRIGHTNESS);

    _initialized = true;
    return true;
}

void DisplayManager::setBrightness(uint8_t level) {
    if (!_initialized) {
        return;
    }
    _display.setBrightness(level);
}

LGFX& DisplayManager::getDisplay() {
    return _display;
}
