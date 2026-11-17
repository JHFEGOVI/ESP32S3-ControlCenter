#include "StorageManager.h"

#include <SD_MMC.h>

#include "Config.h"

bool StorageManager::begin() {
    if (_mounted) {
        return true;
    }

    // SDIO 4-bit nativo: sin setPins() el core usa los pines por defecto del
    // chip y el montaje falla sin devolver un error útil.
    if (!SD_MMC.setPins(Config::SD_CLK, Config::SD_CMD, Config::SD_D0,
                        Config::SD_D1, Config::SD_D2, Config::SD_D3)) {
        return false;
    }

    // mode1bit = false: usa las cuatro líneas de datos del bus SDIO.
    _mounted = SD_MMC.begin("/sdcard", false);
    if (!_mounted) {
        // Libera el bus SDIO tras un montaje fallido: si queda inicializado,
        // un futuro reintento de begin() no puede reconfigurarlo.
        SD_MMC.end();
    }
    return _mounted;
}

bool StorageManager::isMounted() const {
    return _mounted;
}
