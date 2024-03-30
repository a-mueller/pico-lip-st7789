# ST7789 library

I needed this to drive on of those displays with a pico via LVGL and is mostly copied together from other projects on the internet.

## Get started

1. Add the library to your project `git submodule add git@github.com:a-mueller/pico-lip-st7789.git`
2. Include it in your CMakeLists.txt `add_subdirectory(pico-lip-st7789/pico_st7789 build)`
3. Include it in your CMakeLists.txt libraries `target_link_libraries(${NAME} pico_stdlib pico_st7789)`

## Quick start

See the `/example` folder, for LVGL you would want to use the `ST7789::setArea(int x1, int y1, int x2, int y2, uint16_t *buffer)` in the LVGL render callback.
