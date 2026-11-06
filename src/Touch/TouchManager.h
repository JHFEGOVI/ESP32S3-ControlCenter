#pragma once

#include <cstdint>

class LGFX;

class TouchManager {
public:
    explicit TouchManager(LGFX& pantalla);

    bool begin();
    bool detectarToque(uint16_t& x, uint16_t& y);

private:
    LGFX& _gfx;
};
