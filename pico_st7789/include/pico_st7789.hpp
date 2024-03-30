#pragma once

#include <hardware/spi.h>
#include "hardware/gpio.h"

#define MADCTL_MY 0x80
#define MADCTL_MX 0x40
#define MADCTL_MV 0x20
#define MADCTL_ML 0x10
#define MADCTL_RGB 0x00

class ST7789 {
private:
    spi_inst_t *spi;
    /** SPIO cable select, will be low when we talk to the display */
    int gpio_cs;
    /** command vs data. If low we are sending a command, if high we are sending data */
    int gpio_dc;
    /** Hardware reset, should be high in normal operation, low for a short time when initiating the reset */
    int gpio_rst;
    /** Backlight pin. Will be set high when the display is enabled */
    int gpio_bl;
    /** SPIO CoPi, out signal of the MCU sending to the display */
    int gpio_tx;
    /** SPIO clock */
    int gpio_sck;
    /** width at rotation 0 **/
    int display_width;
    /** height at rotation 0 **/
    int display_height;
    /** width, but taken into account the rotation */
    int rotated_width;
    /** height, but taken into account the rotation */
    int rotated_height;
    /** offset of the display 0,0 at default rotation (x) **/
    int offset_x = 0;
    /** offset of the display 0,0 at default rotation (y) **/
    int offset_y = 0;

    /** same as above but using the right value for the current rotation */
    int rotated_offset_x = 0;

    /** same as above but using the right value for the current rotation */
    int rotated_offset_y = 0;

    /** last set rotation (possible values 0,1,2,3) **/
    int rotation = 0;


    /**
     * \brief sends a command to the display
     */
    void cmd(uint8_t cmd, const uint8_t *data = nullptr, size_t len = 0);

    /**
     * \brief column select, together with raset this defines the area we are writing to start - end in both dimensions
     */
    void caset(uint16_t xs, uint16_t xe);

    /**
     * \brief row select, together with caset this defines the area we are writing to start - end in both dimensions
     */
    void raset(uint16_t ys, uint16_t ye);

    /**
     * \brief write to the pixel memory of the display
     */
    void ramwr();

public:
    /** Initializes the Display, doesn't switch it on so you can put something on the screen before it comes on **/
    void init(spi_inst_t *spi, int width, int height, int gpio_cs, int gpio_dc, int gpio_rst, int gpio_bl, int gpio_tx,
              int gpio_sck);

    /** Fills the whole display with a single color **/
    void fill(uint16_t pixel);

    /** Sets the rotation of the screen (from 0 to 3), if you are using lvgl make sure you set the width & height for lvgl to the rotated, not the orignal aspect ratio **/
    void setRotation(int rotation);

    /** offsets for x and y, my display wants to start at pixel 20 for some reason (maybe the round corners or something) **/
    void setDisplayOffset(int x, int y);

    /** sets a single pixel, this will be very slow if there is more than one **/
    void set(int x, int y, uint16_t pixel);

    /** sets an area to the color values in the buffer. Use this in the lvgl drawing callback **/
    void setArea(int x1, int y1, int x2, int y2, uint16_t *buffer);

    /** switches the display on and off (the chip won't go into power saving or anything, just the backlight) **/
    void switchDisplay(bool on);
};
