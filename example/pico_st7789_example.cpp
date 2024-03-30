#include <cstdio>

#include "pico/stdlib.h"
#include "pico_st7789.hpp"


int main() {
    stdio_init_all();
    ST7789 st7789;
    st7789.init(PICO_DEFAULT_SPI_INSTANCE, 240, 280, 17, 20, 21, 22, 19, 18);
    st7789.switchDisplay(true);

    sleep_ms(1000);

    st7789.fill(0xffff);
    while (true) {
        st7789.fill(0x0000);
        sleep_ms(1000);
        st7789.fill(0x1e16);
        sleep_ms(1000);
    }

}

