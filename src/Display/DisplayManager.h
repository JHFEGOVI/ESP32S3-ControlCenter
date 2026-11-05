#pragma once

#include <cstdint>

#include "lgfx_config.hpp"

class DisplayManager {
public:
    DisplayManager() = default;
    ~DisplayManager() = default;

    DisplayManager(const DisplayManager&) = delete;
    DisplayManager& operator=(const DisplayManager&) = delete;

    bool begin();
    void setBrightness(uint8_t level);
    LGFX& getDisplay();

private:
    LGFX _display;
    bool _initialized = false;
};
