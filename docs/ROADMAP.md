# Roadmap

Estado del proyecto y alcance previsto. Las casillas se actualizan cuando la funcionalidad está probada en hardware, no solo escrita.

## Fase 0 - Base

- [x] Repositorio y licencia MIT.
- [x] `platformio.ini` con ESP32-S3 N16R8, 16 MB flash y PSRAM OPI.
- [x] Estructura de carpetas en `src/`.
- [x] Documentación inicial.
- [ ] `Config.h` con el pinout definitivo.
- [ ] Esqueleto de módulos con interfaces públicas.

## Fase 1 - Bring-up de hardware

- [ ] USB CDC estable y log de arranque.
- [ ] Detección del FT6336G por I2C.
- [ ] Inicialización del ILI9341V y retroiluminación.
- [ ] Dibujo de prueba y calibración táctil.
- [ ] Montaje y lectura de la microSD.
- [ ] Buzzer con patrones básicos.
- [ ] Prueba de estabilidad con todos los periféricos activos.

## Fase 2 - Framework de UI

- [ ] Pantalla base con LovyanGFX y sprites.
- [ ] Widgets: botón, indicador, barra de progreso, lista.
- [ ] Máquina de estados de pantallas.
- [ ] Redibujado por regiones sucias.
- [ ] Navegación táctil con debounce.
- [ ] Pantalla de error y modo degradado.

## Fase 3 - Módulos funcionales

- [ ] Monitor de CPU y RAM.
- [ ] Control multimedia.
- [ ] Ejecución de scripts.
- [ ] Temporizador Pomodoro.
- [ ] Notificaciones con buzzer.
- [ ] Carga de iconos y fuentes desde la SD.

## Fase 4 - Integración con Linux

- [ ] Definir el protocolo serial (tramas, versión, CRC).
- [ ] Agente de host para leer métricas del sistema.
- [ ] Puente para comandos multimedia.
- [ ] Ejecución controlada de scripts.
- [ ] Reconexión automática tras suspensión o reinicio del PC.

## Fase 5 - Robustez

- [ ] Timeouts y reintentos en todos los buses.
- [ ] Watchdog y recuperación de fallos.
- [ ] Pruebas prolongadas de memoria y estabilidad.
- [ ] Perfilado de consumo.
- [ ] Documentación del protocolo y del pinout final.
- [ ] Primer release etiquetado.

## Fuera de alcance por ahora

- Aplicación móvil.
- Wi-Fi o Bluetooth.
- Actualización OTA.
- Soporte para pantallas distintas a la 2.8" actual.
