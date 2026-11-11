# Instrucciones para el agente

Este archivo define cómo trabajar en este repositorio. Léelo completo antes de
responder. Si una petición del usuario entra en conflicto con estas reglas,
señálalo y pide aclaración antes de implementar.

## Contexto obligatorio

Antes de escribir código, lee:

- `README.md` — propósito, hardware y stack.
- `docs/BUENAS_PRACTICAS.md` — reglas 1–10 de código y arquitectura.
- `docs/ARQUITECTURA.md` — capas y responsabilidades.
- `docs/HARDWARE.md` — pines, buses y periféricos.
- `docs/ROADMAP.md` — fase actual y alcance.
- `include/Config.h` — constantes y pines.
- `platformio.ini` — placa, framework y dependencias.

## Regla principal: incremental y con consulta

- **Un módulo por sesión.** No escribas varios managers o módulos en la misma
  respuesta. Si el usuario pide `StorageManager`, no añadas `Buzzer`, `UI` ni
  `Serial`.
- **Pide confirmación antes de cada paso grande.** Antes de escribir un archivo
  nuevo, refactorizar o tocar más de un archivo, propone el plan en 3–5 líneas
  y espera el OK.
- **No te adelantes al roadmap.** Trabaja solo en la casilla actual de la fase
  actual. Si ves algo de fases futuras, menciónalo, pero no lo implementes.
- **No inventes pines, direcciones I2C, frecuencias ni constantes.** Si falta
  un dato en `Config.h` o en `docs/HARDWARE.md`, pregunta antes de asumir.
- **Archivos completos, no fragmentos.** Cuando escribas código, entrega el
  archivo entero (`.h` y `.cpp`) para que el usuario lo pegue y compile.
- **No ejecutes Git.** El usuario hace los commits. Tú solo escribes código y
  reportas el resultado de compilar.

## Reglas de código (resumen de docs/BUENAS_PRACTICAS.md)

- **Desacoplamiento:** `Modules/` nunca incluye `UI/` ni `Display/`. Los
  módulos producen datos; la UI los dibuja.
- **No bloqueante:** prohibido `delay()` en el flujo principal. Temporizadores
  con `millis()`. `delay(20)` como máximo al final de `loop()`.
- **Memoria:** sin `String` ni `new`/`delete` en `loop()`. Buffers `char[]` y
  `snprintf()`. PSRAM para recursos grandes, con comprobación de `nullptr`.
- **Redibujado selectivo:** nunca `fillScreen()` en cada iteración. Dirty flags
  por región o widget.
- **Constantes:** todo pin, tiempo, color y tamaño vive en `Config.h` con
  `constexpr`. Sin números mágicos en `src/`.
- **`main.cpp` delgado:** `setup()` inicializa, `loop()` despacha. Si crece,
  la lógica pertenece a un módulo.
- **Logging:** nada de `Serial.println()` suelto en código de producción. Usar
  macros o logger con nivel y etiqueta de módulo.
- **Includes:** entre carpetas, `#include "Carpeta/Archivo.h"`, nunca rutas
  relativas `../`.
- **Nombres:** clases `PascalCase`, métodos y variables `camelCase`,
  constantes `UPPER_SNAKE_CASE`.

Si una petición del usuario incumple una de estas reglas (un `delay()` largo,
un `String` en `loop()`, un `fillScreen()` por ciclo, un pin hardcodeado),
señálalo y propone la alternativa correcta **antes** de implementar.

## Flujo de trabajo

1. El usuario indica el módulo o la casilla actual del roadmap.
2. Propón el plan: archivos a tocar, API pública, criterio de validación en
   placa.
3. Espera el OK.
4. Escribe el código (archivos completos).
5. Recuerda compilar (`pio run`) y validar en hardware.
6. Solo tras validación, sugiere el mensaje de commit (el usuario lo ejecuta).

## Criterio de "terminado"

Un módulo no está terminado cuando compila. Está terminado cuando:

- Compila sin warnings nuevos (`pio run`).
- Se ha validado en placa con un test explícito.
- Cumple las reglas 1–10 de `docs/BUENAS_PRACTICAS.md`.
- El usuario confirma la validación.

No marques casillas del roadmap ni des un módulo por cerrado sin esa
confirmación.

## Qué NO hacer

- No crear archivos de fases futuras "para dejar preparado".
- No refactorizar código validado sin que el usuario lo pida.
- No añadir dependencias a `platformio.ini` sin consultar.
- No asumir que algo funciona: si no está probado en placa, no está terminado.
- No duplicar reglas de `docs/BUENAS_PRACTICAS.md` en los archivos de código;
  referéncialas.
- No inventar APIs de LovyanGFX ni firmas de funciones. Si no estás seguro,
  pregunta o consulta la documentación.