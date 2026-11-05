#pragma once

#include <Config.h>
#include <LovyanGFX.hpp>

// Configuración de hardware del panel. No contiene lógica de dibujo.
class LGFX final : public lgfx::LGFX_Device {
public:
    LGFX();

private:
    lgfx::Bus_SPI _bus;
    lgfx::Panel_ILI9341 _panel;
    lgfx::Light_PWM _light;
    lgfx::Touch_FT5x06 _touch;
};

inline LGFX::LGFX() {
    {
        auto cfg = _bus.config();
        cfg.spi_host = SPI2_HOST;
        cfg.spi_mode = 0;
        cfg.spi_3wire = false;
        cfg.freq_write = Config::DISPLAY_SPI_FREQ_HZ;
        cfg.pin_sclk = Config::DISPLAY_SCLK;
        cfg.pin_mosi = Config::DISPLAY_MOSI;
        cfg.pin_miso = Config::DISPLAY_MISO;
        cfg.pin_dc = Config::DISPLAY_DC;
        _bus.config(cfg);
        _panel.setBus(&_bus);
    }

    {
        auto cfg = _panel.config();
        cfg.pin_cs = Config::DISPLAY_CS;
        cfg.pin_rst = Config::DISPLAY_RST;
        cfg.pin_busy = -1;

        // Dimensiones nativas en retrato; setRotation(1) las intercambia.
        cfg.memory_width = Config::DISPLAY_NATIVE_WIDTH;
        cfg.memory_height = Config::DISPLAY_NATIVE_HEIGHT;
        cfg.panel_width = Config::DISPLAY_NATIVE_WIDTH;
        cfg.panel_height = Config::DISPLAY_NATIVE_HEIGHT;

        cfg.offset_x = 0;
        cfg.offset_y = 0;
        cfg.offset_rotation = 0;

        cfg.dummy_read_pixel = 8;
        cfg.dummy_read_bits = 1;
        cfg.readable = true;

        // Ajustar en placa si el panel IPS muestra colores invertidos
        // o si los canales rojo y azul aparecen intercambiados.
        cfg.invert = true;
        cfg.rgb_order = false;

        cfg.dlen_16bit = false;
        cfg.bus_shared = true;

        _panel.config(cfg);
    }

    {
        auto cfg = _light.config();
        cfg.pin_bl = Config::DISPLAY_BACKLIGHT;
        cfg.freq = 44100;
        cfg.pwm_channel = 7;
        cfg.offset = 0;
        cfg.invert = false;
        _light.config(cfg);
        _panel.setLight(&_light);
    }

    {
        auto cfg = _touch.config();
        cfg.i2c_port = I2C_NUM_0;
        cfg.i2c_addr = Config::TOUCH_I2C_ADDR;
        cfg.pin_sda = Config::TOUCH_SDA;
        cfg.pin_scl = Config::TOUCH_SCL;
        cfg.pin_int = Config::TOUCH_INT;
        cfg.pin_rst = Config::TOUCH_RST;
        cfg.freq = Config::TOUCH_I2C_FREQ_HZ;

        // El FT6336G entrega coordenadas en la orientación nativa del panel.
        cfg.x_min = 0;
        cfg.x_max = Config::DISPLAY_NATIVE_WIDTH - 1;
        cfg.y_min = 0;
        cfg.y_max = Config::DISPLAY_NATIVE_HEIGHT - 1;
        cfg.offset_rotation = 0;
        cfg.bus_shared = false;

        _touch.config(cfg);
        _panel.setTouch(&_touch);
    }

    setPanel(&_panel);
}
