#include "TouchManager.h"

#include "Display/lgfx_config.hpp"

TouchManager::TouchManager(LGFX& pantalla) : _gfx(pantalla) {}

bool TouchManager::begin() {
    if (_gfx.touch() == nullptr) {
        return false;
    }

    // LovyanGFX no expone un código de error en getTouch(): devuelve la
    // cantidad de puntos detectados. Un 0 es una respuesta válida sin toque,
    // no un fallo del controlador. La lectura comprueba que la ruta responde.
    uint16_t x = 0;
    uint16_t y = 0;
    (void)_gfx.getTouch(&x, &y);

    return true;
}

bool TouchManager::detectarToque(uint16_t& x, uint16_t& y) {
    // LovyanGFX aplica la rotación y la calibración configuradas en LGFX.
    return _gfx.getTouch(&x, &y) > 0;
}
