# Buenas prácticas

Reglas de trabajo para el firmware. No son dogmas, pero cualquier excepción debe tener una razón técnica y quedar comentada en el código.

## 1. Módulos desacoplados

Un módulo solo expone lo necesario. Si `UI` necesita mostrar la CPU, llama a `MonitorModule::getCpuUsage()`; no lee variables internas ni incluye headers de implementación.

- Interfaces públicas en el header.
- Estado interno privado.
- Dependencias en una sola dirección.
- `main.cpp` conecta, no implementa.

## 2. Nada bloqueante

No usar `delay()` en el flujo principal. Tampoco bucles `while` que esperen hardware.

```cpp
// Mal
delay(1000);
leerSensor();

// Bien
if (now - lastRead >= READ_INTERVAL_MS) {
    lastRead = now;
    leerSensor();
}
```

Aplicar el mismo patrón a debounce, animaciones, Pomodoro, timeouts seriales y reintentos de SD.

## 3. Memoria predecible

Evitar `String`, `std::vector` y `new`/`delete` en el `loop()`. La fragmentación del heap en un equipo que queda encendido durante días es un problema real.

- Buffers `char[]` de tamaño fijo.
- `snprintf()` para formatear.
- Arrays estáticos o reservados una sola vez.
- PSRAM para sprites y buffers grandes, con verificación de `nullptr`.
- Literales con `F()`.

## 4. Redibujado selectivo

La pantalla no se borra completa en cada iteración. Cada widget conoce su región y su marca de sucio.

```cpp
if (dirty.cpu) {
    drawCpuRegion();
    dirty.cpu = false;
}
```

La UI usa una FSM para saber qué pantalla está activa. Al cambiar de pantalla se invalida todo; dentro de una pantalla solo se invalida lo que cambió.

## 5. Resiliencia de hardware

Todo acceso a un periférico puede fallar. El firmware debe detectarlo y seguir.

- Timeout en I2C y SPI.
- Verificación de presencia al inicializar.
- Reintento con backoff en SD y táctil.
- Modo degradado visible en la UI.
- Watchdog habilitado en producción si el proyecto lo requiere.

Un periférico ausente no debe reiniciar el equipo en bucle.

## 6. Constantes en `Config.h`

Pines, intervalos, colores, tamaños de buffer, direcciones I2C y comandos viven en un solo lugar.

```cpp
namespace Config {
constexpr uint8_t TOUCH_SDA = 16;
constexpr uint8_t TOUCH_SCL = 15;
constexpr uint32_t SERIAL_BAUD = 115200;
constexpr uint32_t TOUCH_POLL_MS = 20;
}
```

Si un número aparece dos veces en el código, probablemente debería ser una constante.

## 7. `main.cpp` delgado

`setup()` inicializa y `loop()` despacha. Si `main.cpp` empieza a tener condicionales de negocio, esa lógica pertenece a un módulo.

```cpp
void setup() {
    Serial.begin(Config::SERIAL_BAUD);
    display.begin();
    touch.begin();
    storage.begin();
    ui.begin();
}

void loop() {
    const uint32_t now = millis();
    touch.tick(now);
    serial.tick(now);
    ui.tick(now);
    modules.tick(now);
    display.tick(now);
}
```

## 8. Logging con niveles

No dejar `Serial.println()` sueltos. Usar macros o una clase de log con nivel y etiqueta de módulo.

```cpp
LOG_INFO("TOUCH", "Controlador detectado en 0x38");
LOG_WARN("SD", "Sin tarjeta; se usan recursos internos");
LOG_ERROR("SERIAL", "Trama inválida: CRC");
```

El nivel `DEBUG` se compila fuera en producción. Los logs no deben imprimir en rutas de alta frecuencia sin control: llenan el buffer y afectan la latencia.

## 9. Nombres y estilo

- Clases en `PascalCase`; métodos y variables en `camelCase`; constantes en `UPPER_SNAKE_CASE`.
- Prefijar el módulo en los logs.
- Un archivo `.h` y uno `.cpp` por módulo cuando la implementación lo justifique.
- Comentarios para el porqué, no para repetir lo que hace el código.
- Mantener el formato existente del proyecto.

## 10. Antes de hacer commit

- Compila sin warnings nuevos.
- No hay `delay()` nuevos en el `loop()`.
- No se agregaron `String` en rutas críticas.
- El cambio funciona sin SD y sin host conectado, o degrada de forma explícita.
- El README o los docs se actualizan si cambió el hardware o el protocolo.
