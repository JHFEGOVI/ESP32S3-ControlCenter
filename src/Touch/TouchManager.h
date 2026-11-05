#pragma once

#include <cstdint>

class TouchManager {
public:
    bool inicializarTactil();
    bool detectarToque(uint16_t& coordenadaX, uint16_t& coordenadaY);

private:
    bool leerPuntoCrudo(uint16_t& xCrudo, uint16_t& yCrudo);

    bool inicializado_ = false;
};
