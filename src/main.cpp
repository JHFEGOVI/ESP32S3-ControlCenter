#include <Arduino.h>
#include <SD_MMC.h>

#include <cstring>

#include "Storage/StorageManager.h"

namespace {

StorageManager almacenamiento;

// Constantes locales del test de bring-up: no son configuración de producción.
constexpr size_t SD_TEST_PATH_MAX = 256;
constexpr size_t SD_TEST_READ_CHUNK = 256;
constexpr uint32_t BOOT_HEARTBEAT_MS = 2000;

uint32_t ultimoLatido = 0;

bool tieneExtensionBmp(const char* nombre) {
    const size_t longitud = strlen(nombre);
    if (longitud < 4) {
        return false;
    }

    const char* extension = nombre + longitud - 4;
    return strcmp(extension, ".bmp") == 0 || strcmp(extension, ".BMP") == 0;
}

const char* nombreBase(const char* ruta) {
    const char* separador = strrchr(ruta, '/');
    return (separador != nullptr) ? separador + 1 : ruta;
}

void copiarRutaCompleta(const char* nombre, char* destino, size_t destinoSize) {
    if (nombre[0] == '/') {
        snprintf(destino, destinoSize, "%s", nombre);
    } else {
        snprintf(destino, destinoSize, "/%s", nombre);
    }
}

// Recorre la raíz y deja en primeraRuta el primer .bmp encontrado; si no hay
// ningún .bmp, deja el primer archivo que aparezca.
bool listarRaiz(char* primeraRuta, size_t primeraRutaSize) {
    primeraRuta[0] = '\0';

    File raiz = SD_MMC.open("/");
    if (!raiz) {
        Serial.println("SD: no se pudo abrir la raíz");
        return false;
    }
    if (!raiz.isDirectory()) {
        Serial.println("SD: la raíz no es un directorio");
        raiz.close();
        return false;
    }

    Serial.println("SD: listado de la raíz");

    uint16_t archivos = 0;
    bool bmpElegido = false;

    File entrada = raiz.openNextFile();
    while (entrada) {
        const char* nombre = entrada.name();
        const char* base = nombreBase(nombre);

        if (entrada.isDirectory()) {
            Serial.printf("SD: <dir> %s\n", base);
        } else {
            ++archivos;
            Serial.printf("SD: %s, %lu bytes\n", base,
                          static_cast<unsigned long>(entrada.size()));

            const bool esBmp = tieneExtensionBmp(base);
            if (primeraRuta[0] == '\0' || (esBmp && !bmpElegido)) {
                copiarRutaCompleta(nombre, primeraRuta, primeraRutaSize);
                bmpElegido = esBmp;
            }
        }

        entrada.close();
        entrada = raiz.openNextFile();
    }

    raiz.close();

    if (archivos == 0) {
        Serial.println("SD: raíz vacía");
        return false;
    }

    return primeraRuta[0] != '\0';
}

// Un solo bloque acotado basta para validar que el contenido es legible.
void leerArchivo(const char* ruta) {
    File archivo = SD_MMC.open(ruta);
    if (!archivo) {
        Serial.println("SD: no se pudo leer ningún archivo");
        return;
    }

    uint8_t buffer[SD_TEST_READ_CHUNK];
    const size_t leidos = archivo.read(buffer, sizeof(buffer));
    archivo.close();

    Serial.printf("SD: leído %s, %u bytes\n", ruta,
                  static_cast<unsigned>(leidos));
}

}  // namespace

void setup() {
    Serial.begin(115200);

    // USB CDC tarda en enumerar; sin esta espera los primeros prints se pierden.
    delay(2000);
    Serial.println("BOOT: inicio");

    Serial.println("BOOT: montando SD...");
    const bool montada = almacenamiento.begin();
    Serial.printf("BOOT: storage.begin() -> %d\n", montada ? 1 : 0);

    if (!montada) {
        Serial.println("SD: sin tarjeta o fallo de montaje");
        return;
    }

    Serial.println("SD: montada");

    char ruta[SD_TEST_PATH_MAX];
    if (listarRaiz(ruta, sizeof(ruta))) {
        leerArchivo(ruta);
    } else {
        Serial.println("SD: no se pudo leer ningún archivo");
    }
}

void loop() {
    // Heartbeat no bloqueante: demuestra que el loop sigue vivo sin SD.
    const uint32_t ahora = millis();
    if (ahora - ultimoLatido >= BOOT_HEARTBEAT_MS) {
        ultimoLatido = ahora;
        Serial.println("BOOT: vivo");
    }
}
