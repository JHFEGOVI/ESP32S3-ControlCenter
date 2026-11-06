# Roadmap

Estado del proyecto y alcance previsto. Las casillas se actualizan cuando la
funcionalidad está **probada en hardware**, no solo escrita.

Un roadmap de hobby es una brújula, no un contrato. Se revisa cada 4–6 semanas
y se ajusta sin culpa. Solo se mira la fase actual y la siguiente; el resto es
contexto.

## Fase 0 - Base

- [x] Repositorio y licencia MIT.
- [x] `platformio.ini` con ESP32-S3 N16R8, 16 MB flash y PSRAM OPI.
- [x] Estructura de carpetas en `src/`.
- [x] Documentación inicial (`docs/BUENAS_PRACTICAS.md`, `docs/HARDWARE.md`).
- [ ] `Config.h` con el pinout definitivo (`constexpr`, agrupado por bus).
- [ ] Esqueletos de módulos con interfaces públicas (solo `.h`, sin lógica).

## Fase 1 - Bring-up de hardware

Orden estricto: cada casilla se valida en placa antes de pasar a la siguiente.

- [ ] USB CDC estable y log de arranque.
- [x] Inicialización del ILI9341V y retroiluminación (backlight ON).
- [x] Dibujo de prueba: color, formas, texto y un sprite pequeño.
- [ ] Detección del FT6336G por I2C y mapeo de coordenadas a la rotación del display.
- [ ] Montaje y lectura de la microSD (incluye arranque sin SD).
- [ ] Buzzer con patrones básicos.
- [ ] Prueba de estabilidad con todos los periféricos activos.

## Fase 2 - Framework de UI

- [ ] Pantalla base con LovyanGFX y sprites.
- [ ] Widgets: botón, indicador, barra de progreso, lista.
- [ ] Máquina de estados de pantallas.
- [ ] Redibujado por regiones sucias (dirty flags).
- [ ] Navegación táctil con debounce.
- [ ] Pantalla de error y modo degradado (sin SD, sin serial).

## Fase 3 - Módulos funcionales

MVP primero: elegir 1–2 módulos y cerrarlos antes de abrir el siguiente.

- [ ] Pomodoro (temporizador con `millis()`, sin bloqueo).
- [ ] Monitor de CPU y RAM (consume datos del host, no los genera).
- [ ] Stream Deck: ejecución de scripts vía host.
- [ ] Control multimedia.
- [ ] Carga de iconos y fuentes desde la SD.
- [ ] Notificaciones con buzzer.

## Fase 4 - Integración con Linux (sub-proyecto paralelo)

El agente de host es un **segundo repositorio** (Python). No forma parte del
firmware; se desarrolla y versiona aparte.

- [ ] Definir protocolo serial: trama con longitud + `\n`, checksum simple (XOR).
- [ ] Agente de host: métricas del sistema (`/proc/stat`, `/proc/meminfo`).
- [ ] Puente para comandos multimedia.
- [ ] Ejecución controlada de scripts.
- [ ] Reconexión automática tras suspensión o reinicio del PC.

## Fase 5 - Robustez (mínimo viable)

- [ ] Timeouts y reintentos en todos los buses.
- [ ] Watchdog y recuperación de fallos.
- [ ] Documentación final del protocolo y pinout.
- [ ] Primer release etiquetado.

## Opcional / algún día

No bloquea el proyecto. Se hace solo si apetece y hay tiempo.

- [ ] Pruebas prolongadas de memoria y estabilidad (24–72 h).
- [ ] Perfilado de consumo (requiere instrumentación externa).
- [ ] FreeRTOS para tareas concurrentes si el `loop()` se queda corto.

## Fuera de alcance por ahora

- Aplicación móvil.
- Wi-Fi o Bluetooth.
- Actualización OTA.
- Soporte para pantallas distintas a la 2.8" actual.
