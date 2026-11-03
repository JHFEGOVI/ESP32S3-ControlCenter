# Arquitectura

El firmware se organiza por responsabilidades. La meta no es tener muchas capas, sino evitar que un cambio en la pantalla obligue a tocar el protocolo serial o la lógica de Pomodoro.

## Vista general

```text
                 USB CDC
                    │
              ┌─────▼─────┐
              │  Serial   │
              └─────┬─────┘
                    │ eventos / comandos
┌─────────┐   ┌─────▼─────┐   ┌───────────┐
│  Touch  │──▶│    UI     │──▶│  Modules  │
└─────────┘   │  (FSM)    │   │ monitor   │
              └─────┬─────┘   │ multimedia│
                    │         │ scripts   │
              ┌─────▼─────┐   │ pomodoro  │
              │  Display  │   └─────┬─────┘
              └───────────┘         │
                              ┌─────▼─────┐
                              │  Serial   │
                              └───────────┘

      Storage y Buzzer son servicios de apoyo
```

`Touch` y `Serial` producen eventos. `UI` los interpreta y actualiza el estado visible. `Modules` ejecuta la acción concreta, que puede implicar responder al host por serial, y `Display` redibuja únicamente la región afectada.

## Responsabilidades

| Módulo | Responsabilidad | No debe hacer |
| --- | --- | --- |
| `main.cpp` | Inicializar módulos y ejecutar el scheduler cooperativo. | Lógica de negocio, dibujado o parseo. |
| `Config.h` | Pines, tiempos, colores, tamaños y constantes del protocolo. | Lógica ni estado mutable. |
| `Display/` | Inicializar LovyanGFX, sprites, backlight y primitivas de dibujo. | Decidir qué pantalla mostrar. |
| `Touch/` | Leer el FT6336G, filtrar rebotes y emitir eventos táctiles. | Dibujar o cambiar de pantalla. |
| `Serial/` | Recibir tramas, validarlas, despachar comandos y enviar respuestas. | Interpretar reglas de negocio. |
| `Storage/` | Montar la SD, listar y cargar recursos, manejar errores de montaje. | Dibujar iconos directamente. |
| `Buzzer/` | Reproducir patrones de sonido solicitados por otros módulos. | Decidir cuándo notificar. |
| `UI/` | Máquina de estados, widgets, layout y marcas de redibujado. | Acceder directo a hardware. |
| `Modules/` | Lógica de CPU/RAM, multimedia, scripts y Pomodoro. | Dibujar o leer GPIO. |

## Flujo de eventos

1. `Touch` detecta un toque válido y genera un evento con coordenadas y tipo.
2. `UI` traduce el evento según la pantalla activa: cambia de vista, mueve la selección o activa un widget.
3. Si la acción requiere trabajo del PC o de un módulo, `UI` llama a la interfaz pública del módulo correspondiente.
4. El módulo actualiza su estado y, si corresponde, encola una respuesta para `Serial`.
5. `UI` marca como sucia la región afectada y `Display` la repinta en el siguiente ciclo.

No hay llamadas directas de `Touch` a `Modules` ni de `Modules` a `Display`.

## Máquina de estados de la UI

Estados previstos:

- `BOOT`: inicialización y diagnóstico.
- `HOME`: menú principal.
- `MONITOR`: CPU, RAM y demás métricas del host.
- `MEDIA`: controles de reproducción.
- `SCRIPTS`: listado y ejecución de scripts.
- `POMODORO`: temporizador y ciclos de trabajo/descanso.
- `ERROR`: fallo recuperable, con opción de reintentar.

Cada estado define tres operaciones: entrar, actualizar y salir. El cambio de estado se solicita con un evento; no se cambia el estado desde dentro del dibujado.

## Scheduler cooperativo

El `loop()` no debe contener esperas. Cada módulo expone un método `tick()` o `update()` que revisa sus propios temporizadores con `millis()`:

```cpp
void loop() {
    touch.tick(now);
    serial.tick(now);
    ui.tick(now);
    modules.tick(now);
    display.tick(now);

    if (buzzer.isBusy()) {
        buzzer.tick(now);
    }
}
```

El orden importa: primero se recogen eventos, luego se actualiza el estado, después se ejecuta la lógica y al final se dibuja. Un `tick()` debe tardar microsegundos o pocos milisegundos; cualquier operación lenta (montaje de SD, escritura de archivos) se hace fuera del camino crítico o se divide en pasos.

## Memoria

- Sin `String` en rutas críticas: usar `char[]` con tamaño fijo y `snprintf`.
- Sin `new`/`delete` después de la inicialización.
- Sprites y buffers grandes en PSRAM; verificar el resultado de la reserva.
- Evitar copias de buffers completos al pasar datos entre módulos; usar referencias o punteros con contrato claro.
- Los literales de log y de UI usan `F()` para no duplicarlos en RAM.

## Errores y diagnóstico

Cada módulo reporta su estado con un código simple (`OK`, `NOT_FOUND`, `TIMEOUT`, `IO_ERROR`). El arranque continúa aunque un periférico falle; la UI muestra el estado degradado en lugar de quedarse bloqueada.

El log serial usa niveles (`ERROR`, `WARN`, `INFO`, `DEBUG`) y se puede recortar en compilación. En producción, `DEBUG` no debería estar activo.

## Enlace con el host

El protocolo serial está pendiente de definición. Requisitos mínimos:

- Tramas delimitadas y con verificación de integridad.
- Versión de protocolo para detectar incompatibilidades.
- Comandos idempotentes y respuestas con estado.
- Timeout y reintento en el host.
- Capacidad de operar sin el host conectado, mostrando el último estado conocido.

La implementación concreta se documentará en `docs/` cuando se congele la primera versión.
