/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"
#include <ti/devices/msp/msp.h>  // For delay_cycles
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// TFT pin defines (from syscfg; use full generated names)
#define TFT_RESET_PORT      GPIO_TFT_RESET_PORT
#define TFT_RESET_PIN       GPIO_TFT_RESET_TFT_RESET_PIN_PIN
#define TFT_DC_PORT         GPIO_TFT_DC_PORT
#define TFT_DC_PIN          GPIO_TFT_DC_TFT_DC_PIN_PIN
#define TFT_CS_PORT         GPIO_TFT_CS_PORT
#define TFT_CS_PIN          GPIO_TFT_CS_TFT_CS_PIN_PIN
#define TFT_SPI_INSTANCE    SPI_1_INST  // Matches syscfg name

// ILI9341 commands
#define ILI9341_SWRESET     0x01
#define ILI9341_SLPOUT      0x11
#define ILI9341_DISPON      0x29
#define ILI9341_CASET       0x2A
#define ILI9341_PASET       0x2B
#define ILI9341_RAMWR       0x2C
#define ILI9341_MADCTL      0x36
#define ILI9341_PIXFMT      0x3A
#define ILI9341_GMCTRP1     0xE0
#define ILI9341_GMCTRN1     0xE1
#define ILI9341_PWCTR1      0xC0
#define ILI9341_PWCTR2      0xC1
#define ILI9341_VMCTR1      0xC5
#define ILI9341_VMCTR2      0xC7
#define ILI9341_FRMCTR1     0xB1
#define ILI9341_DFUNCTR     0xB6
#define ILI9341_READ_ID4    0xD3

// Dimensions
#define TFT_WIDTH  240
#define TFT_HEIGHT 320

// 5x8 font (ASCII 32-127; adjusted to 8 rows for better visibility)
const uint8_t font[96][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, //  
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // "
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // #
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // (
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // )
    {0x08, 0x2A, 0x1C, 0x2A, 0x08}, // *
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // /
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ;
    {0x00, 0x08, 0x14, 0x22, 0x41}, // <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // =
    {0x41, 0x22, 0x14, 0x08, 0x00}, // >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // ?
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // @
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7F, 0x09, 0x09, 0x01, 0x01}, // F
    {0x3E, 0x41, 0x41, 0x51, 0x32}, // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7F, 0x02, 0x04, 0x02, 0x7F}, // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
    {0x7F, 0x20, 0x18, 0x20, 0x7F}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x03, 0x04, 0x78, 0x04, 0x03}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
    {0x00, 0x00, 0x7F, 0x41, 0x41}, // [
    {0x02, 0x04, 0x08, 0x10, 0x20}, // \
    {0x41, 0x41, 0x7F, 0x00, 0x00}, // ]
    {0x04, 0x02, 0x01, 0x02, 0x04}, // ^
    {0x40, 0x40, 0x40, 0x40, 0x40}, // _
    {0x00, 0x01, 0x02, 0x04, 0x00}, // `
    {0x20, 0x54, 0x54, 0x54, 0x78}, // a
    {0x7F, 0x48, 0x44, 0x44, 0x38}, // b
    {0x38, 0x44, 0x44, 0x44, 0x20}, // c
    {0x38, 0x44, 0x44, 0x48, 0x7F}, // d
    {0x38, 0x54, 0x54, 0x54, 0x18}, // e
    {0x08, 0x7E, 0x09, 0x01, 0x02}, // f
    {0x08, 0x14, 0x54, 0x54, 0x3C}, // g
    {0x7F, 0x08, 0x04, 0x04, 0x78}, // h
    {0x00, 0x44, 0x7D, 0x40, 0x00}, // i
    {0x20, 0x40, 0x44, 0x3D, 0x00}, // j
    {0x00, 0x7F, 0x10, 0x28, 0x44}, // k
    {0x00, 0x41, 0x7F, 0x40, 0x00}, // l
    {0x7C, 0x04, 0x18, 0x04, 0x78}, // m
    {0x7C, 0x08, 0x04, 0x04, 0x78}, // n
    {0x38, 0x44, 0x44, 0x44, 0x38}, // o
    {0x7C, 0x14, 0x14, 0x14, 0x08}, // p
    {0x08, 0x14, 0x14, 0x18, 0x7C}, // q
    {0x7C, 0x08, 0x04, 0x04, 0x08}, // r
    {0x48, 0x54, 0x54, 0x54, 0x20}, // s
    {0x04, 0x3F, 0x44, 0x40, 0x20}, // t
    {0x3C, 0x40, 0x40, 0x20, 0x7C}, // u
    {0x1C, 0x20, 0x40, 0x20, 0x1C}, // v
    {0x3C, 0x40, 0x30, 0x40, 0x3C}, // w
    {0x44, 0x28, 0x10, 0x28, 0x44}, // x
    {0x0C, 0x50, 0x50, 0x50, 0x3C}, // y
    {0x44, 0x64, 0x54, 0x4C, 0x44}, // z
    {0x00, 0x08, 0x36, 0x41, 0x00}, // {
    {0x00, 0x00, 0x7F, 0x00, 0x00}, // |
    {0x00, 0x41, 0x36, 0x08, 0x00}, // }
    {0x08, 0x08, 0x2A, 0x1C, 0x08}, // ~
    {0x08, 0x1C, 0x2A, 0x08, 0x08}  // <-
};

// Functions
void tft_spi_send(uint8_t data) {
    DL_SPI_transmitData8(TFT_SPI_INSTANCE, data);
    while (DL_SPI_isBusy(TFT_SPI_INSTANCE));
}

void tft_send_command(uint8_t cmd) {
    DL_GPIO_clearPins(TFT_DC_PORT, TFT_DC_PIN);
    DL_GPIO_clearPins(TFT_CS_PORT, TFT_CS_PIN);
    tft_spi_send(cmd);
    DL_GPIO_setPins(TFT_CS_PORT, TFT_CS_PIN);
}

void tft_send_data(uint8_t data) {
    DL_GPIO_setPins(TFT_DC_PORT, TFT_DC_PIN);
    DL_GPIO_clearPins(TFT_CS_PORT, TFT_CS_PIN);
    tft_spi_send(data);
    DL_GPIO_setPins(TFT_CS_PORT, TFT_CS_PIN);
}

void tft_send_data16(uint16_t data) {
    tft_send_data(data >> 8);
    tft_send_data(data);
}

void tft_init(void) {
    // Reset
    DL_GPIO_clearPins(TFT_RESET_PORT, TFT_RESET_PIN);
    delay_cycles(32 * 1000 * 10);  // 10ms @ 32MHz
    DL_GPIO_setPins(TFT_RESET_PORT, TFT_RESET_PIN);
    delay_cycles(32 * 1000 * 120);  // 120ms

    tft_send_command(ILI9341_SWRESET);
    delay_cycles(32 * 1000 * 150);

    tft_send_command(0xEF); tft_send_data(0x03); tft_send_data(0x80); tft_send_data(0x02);
    tft_send_command(0xCF); tft_send_data(0x00); tft_send_data(0xC1); tft_send_data(0x30);
    tft_send_command(0xED); tft_send_data(0x64); tft_send_data(0x03); tft_send_data(0x12); tft_send_data(0x81);
    tft_send_command(0xE8); tft_send_data(0x85); tft_send_data(0x00); tft_send_data(0x78);
    tft_send_command(0xCB); tft_send_data(0x39); tft_send_data(0x2C); tft_send_data(0x00); tft_send_data(0x34); tft_send_data(0x02);
    tft_send_command(0xF7); tft_send_data(0x20);
    tft_send_command(0xEA); tft_send_data(0x00); tft_send_data(0x00);
    tft_send_command(ILI9341_PWCTR1); tft_send_data(0x23);
    tft_send_command(ILI9341_PWCTR2); tft_send_data(0x10);
    tft_send_command(ILI9341_VMCTR1); tft_send_data(0x3E); tft_send_data(0x28);
    tft_send_command(ILI9341_VMCTR2); tft_send_data(0x86);
    tft_send_command(ILI9341_MADCTL); tft_send_data(0x48);
    tft_send_command(ILI9341_PIXFMT); tft_send_data(0x55);  // 16-bit
    tft_send_command(ILI9341_FRMCTR1); tft_send_data(0x00); tft_send_data(0x18);
    tft_send_command(ILI9341_DFUNCTR); tft_send_data(0x08); tft_send_data(0x82); tft_send_data(0x27);
    tft_send_command(0xF2); tft_send_data(0x00);
    tft_send_command(0x26); tft_send_data(0x01);
    tft_send_command(ILI9341_GMCTRP1); tft_send_data(0x0F); tft_send_data(0x31); tft_send_data(0x2B); tft_send_data(0x0C); tft_send_data(0x0E); tft_send_data(0x08); tft_send_data(0x4E); tft_send_data(0xF1); tft_send_data(0x37); tft_send_data(0x07); tft_send_data(0x10); tft_send_data(0x03); tft_send_data(0x0E); tft_send_data(0x09); tft_send_data(0x00);
    tft_send_command(ILI9341_GMCTRN1); tft_send_data(0x00); tft_send_data(0x0E); tft_send_data(0x14); tft_send_data(0x03); tft_send_data(0x11); tft_send_data(0x07); tft_send_data(0x31); tft_send_data(0xC1); tft_send_data(0x48); tft_send_data(0x08); tft_send_data(0x0F); tft_send_data(0x0C); tft_send_data(0x31); tft_send_data(0x36); tft_send_data(0x0F);
    tft_send_command(ILI9341_SLPOUT);
    delay_cycles(32 * 1000 * 150);
    tft_send_command(ILI9341_DISPON);
    delay_cycles(32 * 1000 * 150);
}

void tft_set_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    tft_send_command(ILI9341_CASET); tft_send_data16(x0); tft_send_data16(x1);
    tft_send_command(ILI9341_PASET); tft_send_data16(y0); tft_send_data16(y1);
    tft_send_command(ILI9341_RAMWR);
}

void tft_fill_screen(uint16_t color) {
    tft_set_addr_window(0, 0, TFT_WIDTH - 1, TFT_HEIGHT - 1);
    DL_GPIO_clearPins(TFT_DC_PORT, TFT_DC_PIN);  // Command mode briefly if needed, but already in data after RAMWR
    DL_GPIO_clearPins(TFT_CS_PORT, TFT_CS_PIN);  // Keep CS low for bulk
    for (uint32_t i = 0; i < (uint32_t)TFT_WIDTH * TFT_HEIGHT; i++) {
        tft_spi_send(color >> 8);
        tft_spi_send(color);
    }
    DL_GPIO_setPins(TFT_CS_PORT, TFT_CS_PIN);
}

void tft_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
    tft_set_addr_window(x, y, x, y);
    tft_send_data16(color);
}

void tft_draw_char(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size) {
    if (c < ' ' || c > '~') return;
    for (uint8_t i = 0; i < 6; i++) {
        uint8_t line = (i == 5) ? 0 : font[(uint8_t)(c - ' ')][i];
        for (uint8_t j = 0; j < 8; j++) {
            uint16_t px = (line & 0x80) ? color : bg;  // Check MSB (bit 7)
            if (size == 1) {
                tft_draw_pixel(x + i, y + j, px);
            } else {
                for (uint8_t k = 0; k < size; k++) {
                    for (uint8_t l = 0; l < size; l++) {
                        tft_draw_pixel(x + i*size + k, y + j*size + l, px);
                    }
                }
            }
            line <<= 1;  // Shift LEFT to get next bit
        }
    }
}

void tft_draw_string(uint16_t x, uint16_t y, const char* str, uint16_t color, uint16_t bg, uint8_t size) {
    uint16_t cursor_x = x;
    for (size_t i = 0; i < strlen(str); i++) {
        tft_draw_char(cursor_x, y, str[i], color, bg, size);
        cursor_x += size * 6;
    }
}

uint32_t tft_read_id(void) {
    tft_send_command(ILI9341_READ_ID4);
    DL_GPIO_clearPins(TFT_CS_PORT, TFT_CS_PIN);
    tft_spi_send(0x00);  // Dummy
    uint8_t id1 = DL_SPI_receiveData8(TFT_SPI_INSTANCE);  // Usually 0x00
    uint8_t id2 = DL_SPI_receiveData8(TFT_SPI_INSTANCE);  // 0x93
    uint8_t id3 = DL_SPI_receiveData8(TFT_SPI_INSTANCE);  // 0x41
    DL_GPIO_setPins(TFT_CS_PORT, TFT_CS_PIN);
    return (id2 << 8) | id3;
}

uint8_t tft_read_reg(uint8_t reg) {
    tft_send_command(reg);
    DL_GPIO_clearPins(TFT_CS_PORT, TFT_CS_PIN);
    uint8_t val = DL_SPI_receiveData8(TFT_SPI_INSTANCE);
    DL_GPIO_setPins(TFT_CS_PORT, TFT_CS_PIN);
    return val;
}

void tft_write_reg(uint8_t reg, uint8_t val) {
    tft_send_command(reg);
    tft_send_data(val);
}

int main(void) {
    SYSCFG_DL_init();  // Init peripherals from syscfg

    // Set initial states
    DL_GPIO_setPins(TFT_CS_PORT, TFT_CS_PIN);  // CS high
    DL_GPIO_setPins(TFT_DC_PORT, TFT_DC_PIN);  // D/C high
    DL_GPIO_setPins(TFT_RESET_PORT, TFT_RESET_PIN);  // RESET high

    // SPI already init'd by SYSCFG_DL_init(); set clock divider (SCR=7 for ~4MHz if prescaler=1)
    DL_SPI_setBitRateSerialClockDivider(TFT_SPI_INSTANCE, 7);
    DL_SPI_enable(TFT_SPI_INSTANCE);

    // Init TFT
    tft_init();


    // Verify: Write/read MADCTL
    tft_write_reg(ILI9341_MADCTL, 0x48);
    uint8_t read_val = tft_read_reg(ILI9341_MADCTL);

    // Read ID (expect 0x9341)
    uint32_t id = tft_read_id();

    bool allCorrect = (read_val == 0x48) && (id == 0x9341);

    // Fill black
    tft_fill_screen(0x0000);

    // Draw text
    tft_draw_string(10, 100, "Hello world", 0xFFFF, 0x0000, 2);

    // Toggle LED based on verification
    if (allCorrect) {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
    } else {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);
    }

    while (1) {
        __WFI();
    }
}