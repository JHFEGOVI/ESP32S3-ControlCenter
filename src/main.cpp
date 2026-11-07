#include <Arduino.h>

#include "Display/DisplayManager.h"
#include "Touch/TouchManager.h"

namespace {

DisplayManager pantalla;
TouchManager tactil(pantalla.getDisplay());

}  // namespace

void setup() {
    Serial.begin(115200);

    if (!pantalla.begin()) {
        Serial.println("Pantalla: fallo de inicialización");
    }

    if (tactil.begin()) {
        Serial.println("Táctil: responde");
    } else {
        Serial.println("Táctil: no responde");
    }
}

void loop() {
    uint16_t x = 0;
    uint16_t y = 0;

    if (tactil.detectarToque(x, y)) {
        Serial.printf("Toque: x=%u, y=%u\n",
                      static_cast<unsigned>(x),
                      static_cast<unsigned>(y));
    }

    delay(20);
}
