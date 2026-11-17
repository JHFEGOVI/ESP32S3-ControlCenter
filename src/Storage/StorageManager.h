#pragma once

class StorageManager {
public:
    StorageManager() = default;
    ~StorageManager() = default;

    StorageManager(const StorageManager&) = delete;
    StorageManager& operator=(const StorageManager&) = delete;

    bool begin();
    bool isMounted() const;

private:
    bool _mounted = false;
};
