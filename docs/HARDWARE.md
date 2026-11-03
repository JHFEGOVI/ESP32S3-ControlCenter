# Hardware

Referencia del diseño físico de ESP32S3-ControlCenter. Los valores definitivos de pines y temporización se declaran en `src/Config.h`; este documento explica el criterio detrás de cada conexión.

## Resumen

| Elemento | Especificación |
| --- | --- |
| MCU | ESP32-S3 ES3C28P, N16R8 (16 MB Flash, 8 MB PSRAM OPI) |
| Pantalla | IPS 2.8", 240x320 px, ILI9341V, SPI |
| Táctil | FT6336G capacitivo, I2C |
| Almacenamiento | microSD, bus SPI compartido con la pantalla |
| Avisos | Buzzer |
| Enlace | USB CDC nativo |

## Pinout

| Señal | Periférico | GPIO | Estado |
| --- | --- | --- | --- |
| SDA | FT6336G | 16 | Fijado |
| SCL | FT6336G | 15 | Fijado |
| SPI SCK / MOSI / MISO | ILI9341V y microSD | Por definir | Pendiente en `Config.h` |
| CS pantalla | ILI9341V | Por definir | Pendiente en `Config.h` |
| CS microSD | microSD | Por definir | Pendiente en `Config.h` |
| DC / RST / BL | ILI9341V | Por definir | Pendiente en `Config.h` |
| Buzzer | Buzzer | Por definir | Pendiente en `Config.h` |

La pantalla y la tarjeta SD comparten el bus SPI. Cada dispositivo debe tener su propia línea de selección (`CS`) y el firmware no debe inicializar ambos a la vez sin liberar antes el bus.

## Buses

### SPI

Se usa para la pantalla y la microSD. La velocidad se ajusta por dispositivo: la pantalla tolera frecuencias altas, mientras que la SD suele necesitar un ritmo más conservador durante la inicialización. LovyanGFX configura el panel; el acceso a la SD se hace aparte.

### I2C

El FT6336G trabaja como esclavo I2C en el bus definido por SDA=16 y SCL=15. El firmware debe:

- Inicializar el bus con pull-ups externos o internos según la placa.
- Verificar la presencia del controlador antes de usarlo.
- Aplicar timeout en cada lectura para no bloquear el `loop()`.
- Reintentar la inicialización si el táctil desaparece.

### USB CDC

La placa se conecta al PC por USB nativo. `ARDUINO_USB_CDC_ON_BOOT=1` hace que `Serial` apunte al puerto CDC, que aparece en Linux como `/dev/ttyACM0` (o el siguiente disponible). No se requiere un adaptador USB-serial adicional.

## Alimentación

- La placa se alimenta por el mismo USB que transporta los datos.
- La lógica es de 3.3 V. No aplicar 5 V a los GPIO.
- La retroiluminación de la pantalla es el consumo más variable; si se controla por PWM, hacerlo desde un GPIO con la corriente adecuada.
- Agregar desacople cerca de la pantalla y del conector SD si aparecen reinicios durante el acceso al bus.

## Almacenamiento

La microSD se formatea en FAT32. La estructura prevista es:

```text
/
├── icons/      # Iconos en formato .bmp
├── fonts/      # Fuentes bitmap
└── config/     # Configuración local, si aplica
```

El firmware debe funcionar sin tarjeta: en ese caso usa los recursos integrados en flash y omite las pantallas que dependan de iconos externos. La SD se monta después de inicializar el bus SPI y se desmonta antes de apagar o reiniciar.

## Buzzer

El tipo de buzzer determina el control:

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

- Confirmar y documentar el pinout completo de SPI, backlight, SD y buzzer.
- Medir el consumo real con pantalla encendida y acceso a SD.
- Validar la estabilidad del bus SPI con pantalla y SD activas al mismo tiempo.
