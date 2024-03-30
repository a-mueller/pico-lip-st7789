#include "pico_st7789.hpp"

#include <cstdio>
#include <pico/time.h>


void ST7789::init(spi_inst_t *spi, int width, int height, int gpio_cs, int gpio_dc, int gpio_rst, int gpio_bl,
                  int gpio_tx, int gpio_sck) {
    this->spi = spi;
    this->gpio_cs = gpio_cs;
    this->gpio_dc = gpio_dc;
    this->gpio_rst = gpio_rst;
    this->gpio_bl = gpio_bl;
    this->gpio_tx = gpio_tx;
    this->gpio_sck = gpio_sck;
    this->display_width = width;
    this->display_height = height;
    this->rotated_width = display_width;
    this->rotated_height = display_height;

    spi_init(spi, 40 * 1000 * 1000);

    gpio_set_function(gpio_tx, GPIO_FUNC_SPI);
    gpio_set_function(gpio_sck, GPIO_FUNC_SPI);

    spi_set_format(spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    // gpio_init(gpio_cs);
    gpio_init(gpio_dc);
    gpio_init(gpio_rst);
    gpio_init(gpio_bl);
    gpio_init(gpio_cs);

    gpio_set_dir(gpio_cs, GPIO_OUT);
    gpio_set_dir(gpio_dc, GPIO_OUT);
    gpio_set_dir(gpio_rst, GPIO_OUT);
    gpio_set_dir(gpio_bl, GPIO_OUT);

    gpio_put(gpio_rst, true);
    gpio_put(gpio_cs, true);
    gpio_put(gpio_dc, true);
    sleep_ms(100);

    // SWRESET (01h): Software Reset
    cmd(0x01);
    sleep_ms(150);

    // SLPOUT (11h): Sleep Out
    cmd(0x11);
    sleep_ms(500);

    // COLMOD (3Ah): Interface Pixel Format
    // - RGB interface color format     = 65K of RGB interface
    // - Control interface color format = 16bit/pixel
    uint8_t color_data[1] = {0x55};
    cmd(0x3a, color_data, 1);
    sleep_ms(10);

    // MADCTL (36h): Memory Data Access Control
    // - Page Address Order            = Top to Bottom
    // - Column Address Order          = Left to Right
    // - Page/Column Order             = Normal Mode
    // - Line Address Order            = LCD Refresh Top to Bottom
    // - RGB/BGR Order                 = RGB
    // - Display Data Latch Data Order = LCD Refresh Left to Right
    uint8_t madctl_data[1] = {0x08};
    cmd(0x36, madctl_data, 1);


    // INVON (21h): Display Inversion On
    cmd(0x21);
    sleep_ms(10);

    // NORON (13h): Normal Display Mode On
    cmd(0x13);
    sleep_ms(10);

    // DISPON (29h): Display On
    cmd(0x29);
    sleep_ms(10);

    setRotation(0);
}

void ST7789::cmd(uint8_t cmd, const uint8_t *data, size_t len) {
    // spi_set_format(spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    // spi_set_format(spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_put(gpio_cs, false);
    gpio_put(gpio_dc, false);

    int result = spi_write_blocking(spi, &cmd, 1);
    printf("Written cmd %d with result %d\n", cmd, result);

    if (len > 0) {
        gpio_put(gpio_dc, true);
        sleep_ms(1);
        spi_write_blocking(spi, data, len);
        // printf("params for cmd %d with result %d\n", cmd, dataResult);
        sleep_ms(1);
    }

    gpio_put(gpio_cs, true);
    gpio_put(gpio_dc, true);
}

void ST7789::fill(uint16_t pixel) {
    int num_pixels = display_width * display_height;

    caset(this->rotated_offset_x, this->rotated_offset_x + rotated_width);
    raset(this->rotated_offset_y, this->rotated_offset_y + rotated_height);

    ramwr();

    spi_set_format(spi, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    sleep_ms(1);

    for (int i = 0; i < num_pixels; i++) {
        spi_write16_blocking(spi, &pixel, 1);
    }
    spi_set_format(spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_put(gpio_cs, true);
}

void ST7789::setArea(int x1, int y1, int x2, int y2, uint16_t *buffer) {
    caset(this->rotated_offset_x + x1, this->rotated_offset_x + x2);
    raset(this->rotated_offset_y + y1, this->rotated_offset_y + y2);
    ramwr();
    spi_set_format(spi, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    const int num_pixels = (x2 - x1 +1) * (y2 - y1 + 1);

    for (int i = 0; i < num_pixels; i++) {
        spi_write16_blocking(spi, buffer, 1);
        buffer++;
    }
    spi_set_format(spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_put(gpio_cs, true);
}


void ST7789::set(const int x, const int y, const uint16_t pixel) {
    caset(this->rotated_offset_x + x, this->rotated_offset_x + x + 1);
    raset(this->rotated_offset_y + y, this->rotated_offset_y + y + 1);
    ramwr();
    spi_set_format(spi, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    spi_write16_blocking(spi, &pixel, 1);
    spi_set_format(spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    gpio_put(gpio_cs, true);
}


void ST7789::caset(uint16_t xs, uint16_t xe) {
    uint8_t data[4] = {
        static_cast<uint8_t>(xs >> 8),
        static_cast<uint8_t>(xs & 0xff),
        static_cast<uint8_t>(xe >> 8),
        static_cast<uint8_t>(xe & 0xff),
    };

    // CASET (2Ah): Column Address Set
    cmd(0x2a, data, 4);
}


void ST7789::raset(uint16_t ys, uint16_t ye) {
    uint8_t data[4] = {
        static_cast<uint8_t>(ys >> 8),
        static_cast<uint8_t>(ys & 0xff),
        static_cast<uint8_t>(ye >> 8),
        static_cast<uint8_t>(ye & 0xff),
    };

    // RASET (2Bh): Row Address Set
    cmd(0x2b, data, 4);
}

void ST7789::ramwr() {
    gpio_put(gpio_cs, false);
    gpio_put(gpio_dc, false);

    // RAMWR (2Ch): Memory Write
    uint8_t cmd = 0x2c;
    spi_write_blocking(spi, &cmd, 1);

    gpio_put(gpio_dc, true);
}

void ST7789::switchDisplay(bool on) {
    if (on) {
        gpio_put(gpio_bl, true);
        cmd(0x29);
    } else {
        cmd(0x28);
        gpio_put(gpio_bl, false);
    }
}

void ST7789::setRotation(int rot) {
    uint8_t madctl = 0;
    this->rotation = rot % 4; // make sure it is between 0 and 3
    switch (rotation) {
        case 0:
            madctl = MADCTL_MX | MADCTL_MY | MADCTL_RGB;
            rotated_offset_x = offset_x;
            rotated_offset_y = offset_y;
            rotated_width = display_width;
            rotated_height = display_height;
            break;
        case 1:
            madctl = MADCTL_MY | MADCTL_MV | MADCTL_RGB;
            rotated_offset_y = offset_x;
            rotated_offset_x = offset_y;
            rotated_height = display_width;
            rotated_width = display_height;
            break;
        case 2:
            madctl = MADCTL_RGB;
            rotated_offset_x = offset_x;
            rotated_offset_y = offset_y;
            rotated_width = display_width;
            rotated_height = display_height;
            break;
        case 3:
            madctl = MADCTL_MX | MADCTL_MV | MADCTL_RGB;
            rotated_offset_y = offset_x;
            rotated_offset_x = offset_y;
            rotated_height = display_width;
            rotated_width = display_height;
            break;
    }

    cmd(0x36, &madctl, 1);
}

void ST7789::setDisplayOffset(int x, int y) {
    this->offset_x = x;
    this->offset_y = y;

    setRotation(rotation); // make sure the offsets are rotated correctly
}
