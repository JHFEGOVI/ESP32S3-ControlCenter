# ESP32S3-ControlCenter

Centro de control táctil para computadores con Linux. El ESP32-S3 se conecta por USB CDC y funciona como un panel de escritorio tipo Stream Deck: métricas de CPU y RAM en tiempo real, control multimedia, ejecución de scripts y herramientas de productividad como un temporizador Pomodoro.

La idea es sencilla: tener siempre a mano una interfaz física para las acciones que se repiten durante la jornada, sin depender de atajos de teclado ni de cambiar de ventana. El PC hace el trabajo pesado; el firmware se ocupa de la interacción, el dibujado y el enlace serial.

> **Estado:** estructura base. La carpeta `src/` ya está dividida por módulos, pero la implementación todavía está en desarrollo.

## Hardware y entorno

| Elemento | Especificación |
| --- | --- |
| Microcontrolador | ESP32-S3 ES3C28P, variante N16R8: 16 MB Flash y 8 MB PSRAM OPI |
| Pantalla | IPS de 2.8" (240x320 px), driver ILI9341V por bus SPI |
| Táctil | Capacitivo FT6336G por I2C (SDA = GPIO16, SCL = GPIO15) |
| Almacenamiento | Ranura microSD para iconos `.bmp`, fuentes y recursos |
| Avisos | Buzzer para notificaciones sonoras |
| Enlace con el PC | USB CDC Serial, conexión permanente |

Entorno de desarrollo:

- Linux, probado en Fedora 44.
- VS Code con la extensión de PlatformIO.
- Framework Arduino.
- LovyanGFX `^1.1.12` para el manejo gráfico.
- Placa PlatformIO `esp32-s3-devkitc-1`, flash de 16 MB, memoria `qio_opi` y `ARDUINO_USB_CDC_ON_BOOT=1`.

## Arquitectura de software

```text
include/
└── Config.h          # pines, tiempos, colores, tamaños y comandos

src/
├── main.cpp          # setup() / loop(): inicialización y scheduler cooperativo
├── Display/          # LovyanGFX: panel ILI9341V, backlight y sprites
├── Touch/            # FT6336G por I2C: lectura, filtrado y eventos táctiles
├── Serial/           # enlace USB CDC: tramas, parseo y comandos del host
├── Storage/          # microSD: iconos, fuentes y configuración
├── Buzzer/           # patrones de sonido y notificaciones
├── UI/               # pantallas, widgets, máquina de estados y redibujado
└── Modules/          # lógica funcional: monitor, multimedia, scripts, Pomodoro
```

El flujo de datos es unidireccional: `Touch` y `Serial` generan eventos, `UI` actualiza el estado, `Modules` ejecuta la acción y `Display` redibuja solo lo que cambió. `Storage` y `Buzzer` son servicios de apoyo.

## Checklist operativo de arquitectura

| Punto | Criterio |
| --- | --- |
| Desacoplamiento | Cada módulo expone una interfaz mínima y no conoce los detalles internos de los demás. `main.cpp` solo conecta las piezas. |
| Diseño no bloqueante | Nada de `delay()` en el flujo principal. Temporizadores, debounce y timeouts con `millis()`; el `loop()` siempre retorna rápido. |
| Gestión de memoria | Sin `String` ni `new`/`delete` en rutas críticas. Buffers `char[]`, arrays de tamaño fijo y `F()` para literales; PSRAM para recursos gráficos. |
| Redibujado selectivo | La UI usa una máquina de estados finitos (FSM) y marcas de región sucia. Solo se redibuja lo que cambió, nunca la pantalla completa en cada iteración. |
| Resiliencia de hardware | Los fallos de I2C, SPI o SD se detectan con timeouts y se reintentan. El equipo debe arrancar en modo degradado si un periférico no responde. |
| Constantes centralizadas | Pines, tiempos, colores, tamaños y comandos viven en `Config.h`. Sin números mágicos repartidos por el código. |
| `main.cpp` delgado | `setup()` inicializa y `loop()` despacha. La lógica de negocio vive en `UI/` y `Modules/`. |
| Logging | Salida serial con niveles y macros que se puedan desactivar en compilación. Nada de `Serial.println()` suelto. |

## Instrucciones rápidas

Requisitos: PlatformIO Core o la extensión de VS Code, cable USB de datos y permisos sobre el puerto serial.

```bash
# Clonar
git clone https://github.com/JHFEGOVI/ESP32S3-ControlCenter.git
cd ESP32S3-ControlCenter

# Compilar
pio run

# Flashear
pio run --target upload

# Abrir la consola serial
pio device monitor -b 115200
```

En VS Code: abrir la carpeta del proyecto, esperar a que PlatformIO termine de indexar y usar los botones **Build**, **Upload** y **Monitor** de la barra inferior.

Si el puerto no aparece o no hay permisos:

```bash
pio device list          # identifica el puerto, normalmente /dev/ttyACM0
sudo usermod -aG dialout "$USER"
```

El cambio de grupo requiere cerrar sesión y volver a entrar.

## Documentación

| Documento | Contenido |
| --- | --- |
| [docs/HARDWARE.md](docs/HARDWARE.md) | Conexionado, buses, alimentación y almacenamiento. |
| [docs/ARQUITECTURA.md](docs/ARQUITECTURA.md) | Capas, responsabilidades de cada módulo y flujo de eventos. |
| [docs/BUENAS_PRACTICAS.md](docs/BUENAS_PRACTICAS.md) | Reglas de código, concurrencia cooperativa y manejo de errores. |
| [docs/ROADMAP.md](docs/ROADMAP.md) | Etapas de desarrollo y alcance pendiente. |

## Licencia

MIT. Ver [LICENSE](LICENSE).
