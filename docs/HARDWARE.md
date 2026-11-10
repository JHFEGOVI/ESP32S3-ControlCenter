# Hardware

Referencia del diseño físico de ESP32S3-ControlCenter. Los valores definitivos de pines y temporización se declaran en `include/Config.h`; este documento explica el criterio detrás de cada conexión.

## Resumen

| Elemento | Especificación |
| --- | --- |
| MCU | ESP32-S3 ES3C28P, N16R8 (16 MB Flash, 8 MB PSRAM OPI) |
| Pantalla | IPS 2.8", 320x240 px (apaisada), ILI9341V, SPI |
| Táctil | FT6336G capacitivo, I2C |
| Almacenamiento | microSD por SDIO 4-bit nativo (`SD_MMC`), bus independiente |
| Avisos | Buzzer externo opcional; audio por códec ES8311 + FM8002E |
| Enlace | USB CDC nativo |

## Pinout

| Señal | Periférico | GPIO | Estado |
| --- | --- | --- | --- |
| SDA | FT6336G y ES8311 | 16 | Confirmado |
| SCL | FT6336G y ES8311 | 15 | Confirmado |
| SPI SCLK | ILI9341V | 12 | Confirmado |
| SPI MOSI | ILI9341V | 11 | Confirmado |
| SPI MISO | ILI9341V | 13 | Confirmado |
| CS pantalla | ILI9341V | 10 | Confirmado |
| DC pantalla | ILI9341V | 46 | Confirmado |
| RST pantalla | ILI9341V | -1 | No conectado |
| Backlight | ILI9341V | 45 | Confirmado |
| Buzzer externo | Opcional | Por definir | IO2, IO3, IO14 o IO21 |

La pantalla usa SPI2. La microSD usa SDIO 4-bit nativo y el audio usa I2S; ninguno comparte bus con la pantalla. El táctil y el códec comparten el bus I2C. Los pines de cada periférico se detallan en las secciones siguientes.

## Buses

### SPI

Se usa exclusivamente para la pantalla ILI9341V. LovyanGFX configura el panel a 40 MHz. La microSD va por SDIO 4-bit nativo y no comparte este bus.

### I2C

El bus I2C definido por SDA=16 y SCL=15 es compartido por dos dispositivos:

- FT6336G, controlador táctil, en la dirección `0x38`.
- ES8311, códec de audio, en la dirección `0x18`.

El firmware debe:

- Inicializar el bus con pull-ups externos o internos según la placa.
- Verificar la presencia de cada controlador antes de usarlo.
- Aplicar timeout en cada lectura para no bloquear el `loop()`.
- Reintentar la inicialización si el táctil o el códec desaparecen.

### USB CDC

La placa se conecta al PC por USB nativo. `ARDUINO_USB_CDC_ON_BOOT=1` hace que `Serial` apunte al puerto CDC, que aparece en Linux como `/dev/ttyACM0` (o el siguiente disponible). No se requiere un adaptador USB-serial adicional.

## Alimentación

- La placa se alimenta por el mismo USB que transporta los datos.
- La lógica es de 3.3 V. No aplicar 5 V a los GPIO.
- La retroiluminación de la pantalla es el consumo más variable; si se controla por PWM, hacerlo desde un GPIO con la corriente adecuada.
- Agregar desacople cerca de la pantalla y del conector SD si aparecen reinicios durante el acceso al bus.

## Micro SD

| Señal | GPIO |
| --- | --- |
| SD_CLK | 38 |
| SD_CMD | 40 |
| SD_D0 | 39 |
| SD_D1 | 41 |
| SD_D2 | 48 |
| SD_D3 | 47 |

SDIO 4-bit nativo. Usar la librería `SD_MMC`, no `SD` (SPI). No usa pin CS y no comparte bus con la pantalla (que va por SPI2).

La tarjeta se formatea en FAT32. La estructura prevista es:

```text
/
├── icons/      # Iconos en formato .bmp
├── fonts/      # Fuentes bitmap
└── config/     # Configuración local, si aplica
```

El firmware debe funcionar sin tarjeta: en ese caso usa los recursos integrados en flash y omite las pantallas que dependan de iconos externos. La SD se inicializa por `SD_MMC`, de forma independiente del bus SPI de la pantalla, y se desmonta antes de apagar o reiniciar.

## Audio

| Señal | GPIO | Dirección |
| --- | --- | --- |
| AUDIO_MCLK | 4 | ESP32 → códec |
| AUDIO_BCLK | 5 | ESP32 → códec |
| AUDIO_LRCK | 7 | ESP32 → códec |
| AUDIO_DOUT | 8 | ESP32 → códec |
| AUDIO_DIN | 6 | Códec → ESP32 (micrófono) |
| AUDIO_EN | 1 | Enable del amplificador, activo en bajo |

Códec ES8311 + amplificador FM8002E. Conector de altavoz externo de 2 pines. El bus I2C del códec está compartido con el táctil (SDA=16, SCL=15, dirección I2C del códec `0x18`).

## Buzzer

La placa no incluye buzzer integrado. Si se usa uno externo, irá en un GPIO libre (IO2, IO3, IO14 o IO21), pendiente de decidir. El control depende del tipo:

- Activo: se enciende y apaga por GPIO.
- Pasivo: requiere una señal PWM o `tone()` con la frecuencia deseada.

Los patrones de notificación se definen en `src/Buzzer/`, no en los módulos que los solicitan.

## Orden de bring-up

1. USB CDC y `Serial`.
2. Bus I2C y detección del FT6336G.
3. Bus SPI, inicialización del ILI9341V y retroiluminación.
4. Montaje de la microSD.
5. Buzzer.
6. Prueba conjunta de buses y consumo.

## Pendientes

- Definir si se usará el buzzer externo opcional y en qué GPIO libre.
- Medir el consumo real con pantalla encendida y acceso a la microSD.
- Validar la convivencia del bus SPI2 de la pantalla con SDIO de la microSD y el bus I2S del códec.
