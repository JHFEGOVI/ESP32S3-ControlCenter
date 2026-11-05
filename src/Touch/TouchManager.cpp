#include "TouchManager.h"

#include <Arduino.h>
#include <Wire.h>

#include <Config.h>

namespace {

constexpr uint8_t REGISTRO_ESTADO = 0x02;
constexpr uint8_t BYTES_POR_LECTURA = 5;  // Estado + XH/XL + YH/YL
constexpr uint8_t MASCARA_PUNTOS = 0x0F;
constexpr uint8_t MASCARA_COORDENADA_ALTA = 0x0F;
constexpr uint8_t INTENTOS_INICIALIZACION = 3;
constexpr uint16_t ESPERA_REINTENTO_MS = 5;

}  // namespace

bool TouchManager::inicializarTactil() {
    if (inicializado_) {
        return true;
    }

    if (!Wire.begin(Config::TOUCH_SDA, Config::TOUCH_SCL, Config::TOUCH_I2C_FREQ_HZ)) {
        return false;
    }
    Wire.setTimeOut(50);

    // El controlador puede tardar en responder tras el encendido.
    for (uint8_t intento = 0; intento < INTENTOS_INICIALIZACION; ++intento) {
        Wire.beginTransmission(Config::TOUCH_I2C_ADDR);
        if (Wire.endTransmission() == 0) {
            inicializado_ = true;
            return true;
        }
        delay(ESPERA_REINTENTO_MS);
    }

    return false;
}

bool TouchManager::leerPuntoCrudo(uint16_t& xCrudo, uint16_t& yCrudo) {
    uint8_t datos[BYTES_POR_LECTURA] = {0};

    Wire.beginTransmission(Config::TOUCH_I2C_ADDR);
    Wire.write(REGISTRO_ESTADO);
    if (Wire.endTransmission(false) != 0) {
        return false;
    }

    const size_t recibidos = Wire.requestFrom(
        static_cast<uint8_t>(Config::TOUCH_I2C_ADDR),
        static_cast<size_t>(BYTES_POR_LECTURA));

    if (recibidos != BYTES_POR_LECTURA) {
        return false;
    }

    for (size_t i = 0; i < BYTES_POR_LECTURA; ++i) {
        datos[i] = static_cast<uint8_t>(Wire.read());
    }

    const uint8_t puntos = datos[0] & MASCARA_PUNTOS;
    if (puntos == 0) {
        return false;
    }

    // FT6336G: XH/XL e YH/YL usan solo los 4 bits bajos del byte alto.
    xCrudo = static_cast<uint16_t>(
        ((datos[1] & MASCARA_COORDENADA_ALTA) << 8) | datos[2]);
    yCrudo = static_cast<uint16_t>(
        ((datos[3] & MASCARA_COORDENADA_ALTA) << 8) | datos[4]);

    // Una lectura fuera del área física se descarta como ruido.
    return xCrudo < Config::DISPLAY_NATIVE_WIDTH &&
           yCrudo < Config::DISPLAY_NATIVE_HEIGHT;
}

bool TouchManager::detectarToque(uint16_t& coordenadaX, uint16_t& coordenadaY) {
    if (!inicializado_) {
        return false;
    }

    uint16_t xCrudo = 0;
    uint16_t yCrudo = 0;
    if (!leerPuntoCrudo(xCrudo, yCrudo)) {
        return false;
    }

    /*
     * El panel está montado en vertical (240x320) y la interfaz se dibuja
     * con rotación 1 (320x240). Para pasar de la matriz física a la lógica:
     *
     *   1. El eje X lógico avanza sobre el eje Y físico.
     *   2. El eje Y lógico crece en sentido contrario al eje X físico.
     *
     *   X_logico = Y_fisico
     *   Y_logico = (240 - 1) - X_fisico
     */
    coordenadaX = yCrudo;
    coordenadaY = static_cast<uint16_t>(
        (Config::DISPLAY_NATIVE_WIDTH - 1) - xCrudo);

    // Protección adicional ante calibración imperfecta o ruido.
    if (coordenadaX >= Config::DISPLAY_WIDTH) {
        coordenadaX = Config::DISPLAY_WIDTH - 1;
    }
    if (coordenadaY >= Config::DISPLAY_HEIGHT) {
        coordenadaY = Config::DISPLAY_HEIGHT - 1;
    }

    return true;
}
