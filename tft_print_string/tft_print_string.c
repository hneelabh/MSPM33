/*
 * TFT ILI9341 Driver for MSPM33 (MSPM0+ Series)
 * 3.2" 240x320 Display via SPI
 * Texas Instruments Code Composer Studio
 */

#include "ti_msp_dl_config.h"
#include <ti/devices/msp/msp.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* ========== Hardware Pin Configuration ========== */
#define TFT_RESET_PORT      GPIO_TFT_RESET_PORT
#define TFT_RESET_PIN       GPIO_TFT_RESET_TFT_RESET_PIN_PIN
#define TFT_DC_PORT         GPIO_TFT_DC_PORT
#define TFT_DC_PIN          GPIO_TFT_DC_TFT_DC_PIN_PIN
#define TFT_CS_PORT         GPIO_TFT_CS_PORT
#define TFT_CS_PIN          GPIO_TFT_CS_TFT_CS_PIN_PIN
#define TFT_SPI_INST        SPI_1_INST

/* ========== ILI9341 Command Definitions ========== */
#define CMD_NOP             0x00
#define CMD_SWRESET         0x01
#define CMD_RDDID           0x04
#define CMD_SLPOUT          0x11
#define CMD_DISPON          0x29
#define CMD_CASET           0x2A
#define CMD_PASET           0x2B
#define CMD_RAMWR           0x2C
#define CMD_MADCTL          0x36
#define CMD_PIXFMT          0x3A
#define CMD_PWCTR1          0xC0
#define CMD_PWCTR2          0xC1
#define CMD_VMCTR1          0xC5
#define CMD_VMCTR2          0xC7
#define CMD_FRMCTR1         0xB1
#define CMD_DFUNCTR         0xB6
#define CMD_GMCTRP1         0xE0
#define CMD_GMCTRN1         0xE1
#define CMD_RDID4           0xD3

/* ========== Display Dimensions ========== */
#define SCREEN_WIDTH        320
#define SCREEN_HEIGHT       240

/* ========== Color Definitions (RGB565) ========== */
#define COLOR_BLACK         0x0000
#define COLOR_WHITE         0xFFFF
#define COLOR_RED           0xF800
#define COLOR_GREEN         0x07E0
#define COLOR_BLUE          0x001F
#define COLOR_YELLOW        0xFFE0
#define COLOR_CYAN          0x07FF
#define COLOR_MAGENTA       0xF81F

/* ========== 5x7 Font (ASCII 32-127) ========== */
static const uint8_t font5x7[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // Space
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
    {0x7E, 0x09, 0x09, 0x09, 0x7E}, // A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
    {0x3E, 0x41, 0x41, 0x51, 0x72}, // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
    {0x00, 0x7F, 0x41, 0x41, 0x00}, // [
    {0x02, 0x04, 0x08, 0x10, 0x20}, // Backslash
    {0x00, 0x41, 0x41, 0x7F, 0x00}, // ]
    {0x04, 0x02, 0x01, 0x02, 0x04}, // ^
    {0x40, 0x40, 0x40, 0x40, 0x40}, // _
    {0x00, 0x01, 0x02, 0x04, 0x00}, // `
    {0x20, 0x54, 0x54, 0x54, 0x78}, // a
    {0x7F, 0x48, 0x44, 0x44, 0x38}, // b
    {0x38, 0x44, 0x44, 0x44, 0x20}, // c
    {0x38, 0x44, 0x44, 0x48, 0x7F}, // d
    {0x38, 0x54, 0x54, 0x54, 0x18}, // e
    {0x08, 0x7E, 0x09, 0x01, 0x02}, // f
    {0x0C, 0x52, 0x52, 0x52, 0x3E}, // g
    {0x7F, 0x08, 0x04, 0x04, 0x78}, // h
    {0x00, 0x44, 0x7D, 0x40, 0x00}, // i
    {0x20, 0x40, 0x44, 0x3D, 0x00}, // j
    {0x7F, 0x10, 0x28, 0x44, 0x00}, // k
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
    {0x10, 0x08, 0x08, 0x10, 0x08}, // ~
    {0x00, 0x00, 0x00, 0x00, 0x00}  // DEL
};

/* ========== Low-Level SPI Functions ========== */

// Send single byte via SPI
static inline void SPI_WriteByte(uint8_t data) {
    DL_SPI_transmitData8(TFT_SPI_INST, data);
    while (DL_SPI_isBusy(TFT_SPI_INST));
}

// Set CS Low
static inline void CS_Low(void) {
    DL_GPIO_clearPins(TFT_CS_PORT, TFT_CS_PIN);
}

// Set CS High
static inline void CS_High(void) {
    DL_GPIO_setPins(TFT_CS_PORT, TFT_CS_PIN);
}

// Set DC Low (Command mode)
static inline void DC_Command(void) {
    DL_GPIO_clearPins(TFT_DC_PORT, TFT_DC_PIN);
}

// Set DC High (Data mode)
static inline void DC_Data(void) {
    DL_GPIO_setPins(TFT_DC_PORT, TFT_DC_PIN);
}

/* ========== TFT Command/Data Functions ========== */

// Send command to TFT
void TFT_WriteCommand(uint8_t cmd) {
    DC_Command();
    CS_Low();
    SPI_WriteByte(cmd);
    CS_High();
}

// Send single data byte to TFT
void TFT_WriteData(uint8_t data) {
    DC_Data();
    CS_Low();
    SPI_WriteByte(data);
    CS_High();
}

// Send 16-bit data (color) to TFT
void TFT_WriteData16(uint16_t data) {
    DC_Data();
    CS_Low();
    SPI_WriteByte(data >> 8);
    SPI_WriteByte(data & 0xFF);
    CS_High();
}

// Write multiple data bytes (for bulk operations)
void TFT_WriteDataBulk(uint8_t *data, uint32_t len) {
    DC_Data();
    CS_Low();
    for (uint32_t i = 0; i < len; i++) {
        SPI_WriteByte(data[i]);
    }
    CS_High();
}

/* ========== TFT Initialization ========== */

void TFT_HardwareReset(void) {
    DL_GPIO_clearPins(TFT_RESET_PORT, TFT_RESET_PIN);
    delay_cycles(32000 * 10);  // 10ms @ 32MHz
    DL_GPIO_setPins(TFT_RESET_PORT, TFT_RESET_PIN);
    delay_cycles(32000 * 120); // 120ms
}

void TFT_Init(void) {
    // Hardware reset
    TFT_HardwareReset();
    
    // Software reset
    TFT_WriteCommand(CMD_SWRESET);
    delay_cycles(32000 * 150); // 150ms
    
    // Power control A
    TFT_WriteCommand(0xCB);
    TFT_WriteData(0x39);
    TFT_WriteData(0x2C);
    TFT_WriteData(0x00);
    TFT_WriteData(0x34);
    TFT_WriteData(0x02);
    
    // Power control B
    TFT_WriteCommand(0xCF);
    TFT_WriteData(0x00);
    TFT_WriteData(0xC1);
    TFT_WriteData(0x30);
    
    // Driver timing control A
    TFT_WriteCommand(0xE8);
    TFT_WriteData(0x85);
    TFT_WriteData(0x00);
    TFT_WriteData(0x78);
    
    // Driver timing control B
    TFT_WriteCommand(0xEA);
    TFT_WriteData(0x00);
    TFT_WriteData(0x00);
    
    // Power on sequence control
    TFT_WriteCommand(0xED);
    TFT_WriteData(0x64);
    TFT_WriteData(0x03);
    TFT_WriteData(0x12);
    TFT_WriteData(0x81);
    
    // Pump ratio control
    TFT_WriteCommand(0xF7);
    TFT_WriteData(0x20);
    
    // Power control 1
    TFT_WriteCommand(CMD_PWCTR1);
    TFT_WriteData(0x23);
    
    // Power control 2
    TFT_WriteCommand(CMD_PWCTR2);
    TFT_WriteData(0x10);
    
    // VCOM control 1
    TFT_WriteCommand(CMD_VMCTR1);
    TFT_WriteData(0x3E);
    TFT_WriteData(0x28);
    
    // VCOM control 2
    TFT_WriteCommand(CMD_VMCTR2);
    TFT_WriteData(0x86);
    
    // Memory access control (rotation/mirroring)
    TFT_WriteCommand(CMD_MADCTL);
    TFT_WriteData(0x48);
    
    // Pixel format (16-bit RGB565)
    TFT_WriteCommand(CMD_PIXFMT);
    TFT_WriteData(0x55);
    
    // Frame rate control
    TFT_WriteCommand(CMD_FRMCTR1);
    TFT_WriteData(0x00);
    TFT_WriteData(0x18);
    
    // Display function control
    TFT_WriteCommand(CMD_DFUNCTR);
    TFT_WriteData(0x08);
    TFT_WriteData(0x82);
    TFT_WriteData(0x27);
    
    // 3Gamma function disable
    TFT_WriteCommand(0xF2);
    TFT_WriteData(0x00);
    
    // Gamma curve
    TFT_WriteCommand(0x26);
    TFT_WriteData(0x01);
    
    // Positive gamma correction
    TFT_WriteCommand(CMD_GMCTRP1);
    TFT_WriteData(0x0F);
    TFT_WriteData(0x31);
    TFT_WriteData(0x2B);
    TFT_WriteData(0x0C);
    TFT_WriteData(0x0E);
    TFT_WriteData(0x08);
    TFT_WriteData(0x4E);
    TFT_WriteData(0xF1);
    TFT_WriteData(0x37);
    TFT_WriteData(0x07);
    TFT_WriteData(0x10);
    TFT_WriteData(0x03);
    TFT_WriteData(0x0E);
    TFT_WriteData(0x09);
    TFT_WriteData(0x00);
    
    // Negative gamma correction
    TFT_WriteCommand(CMD_GMCTRN1);
    TFT_WriteData(0x00);
    TFT_WriteData(0x0E);
    TFT_WriteData(0x14);
    TFT_WriteData(0x03);
    TFT_WriteData(0x11);
    TFT_WriteData(0x07);
    TFT_WriteData(0x31);
    TFT_WriteData(0xC1);
    TFT_WriteData(0x48);
    TFT_WriteData(0x08);
    TFT_WriteData(0x0F);
    TFT_WriteData(0x0C);
    TFT_WriteData(0x31);
    TFT_WriteData(0x36);
    TFT_WriteData(0x0F);
    
    // Exit sleep mode
    TFT_WriteCommand(CMD_SLPOUT);
    delay_cycles(32000 * 120);
    
    // Display on
    TFT_WriteCommand(CMD_DISPON);
    delay_cycles(32000 * 120);
}

/* ========== Register Read/Write for Testing ========== */

uint8_t TFT_ReadRegister(uint8_t reg) {
    TFT_WriteCommand(reg);
    CS_Low();
    DC_Data();
    uint8_t val = DL_SPI_receiveData8(TFT_SPI_INST);
    CS_High();

    // Debug Statement - Print to console
    printf("Register 0x%02X = 0x%02X\n", reg, val);

    return val;
}

uint32_t TFT_ReadID(void) {
    TFT_WriteCommand(CMD_RDID4);
    CS_Low();
    DC_Data();
    SPI_WriteByte(0x00); // Dummy read
    uint8_t id1 = DL_SPI_receiveData8(TFT_SPI_INST);
    uint8_t id2 = DL_SPI_receiveData8(TFT_SPI_INST);
    uint8_t id3 = DL_SPI_receiveData8(TFT_SPI_INST);
    CS_High();
    
    // Debug Statement - Print to console
    printf("Display ID: 0x%04X (id2=0x%02X, id3=0x%02X)\n", id1, id2, id3);

    return ((uint32_t)id2 << 8) | id3;
}

/* ========== Graphics Functions ========== */

void TFT_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // Column address set
    TFT_WriteCommand(CMD_CASET);
    TFT_WriteData16(x0);
    TFT_WriteData16(x1);
    
    // Page address set
    TFT_WriteCommand(CMD_PASET);
    TFT_WriteData16(y0);
    TFT_WriteData16(y1);
    
    // Memory write
    TFT_WriteCommand(CMD_RAMWR);
}

void TFT_FillScreen(uint16_t color) {
    TFT_SetAddressWindow(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
    
    DC_Data();
    CS_Low();
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    
    for (uint32_t i = 0; i < (uint32_t)SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        SPI_WriteByte(hi);
        SPI_WriteByte(lo);
    }
    CS_High();
}

void TFT_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) return;
    
    TFT_SetAddressWindow(x, y, x, y);
    TFT_WriteData16(color);
}

void TFT_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) return;
    if (x + w > SCREEN_WIDTH) w = SCREEN_WIDTH - x;
    if (y + h > SCREEN_HEIGHT) h = SCREEN_HEIGHT - y;
    
    TFT_SetAddressWindow(x, y, x + w - 1, y + h - 1);
    
    DC_Data();
    CS_Low();
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    
    for (uint32_t i = 0; i < (uint32_t)w * h; i++) {
        SPI_WriteByte(hi);
        SPI_WriteByte(lo);
    }
    CS_High();
}

/* ========== Text Drawing Functions ========== */

void TFT_DrawChar(uint16_t x, uint16_t y, char ch, uint16_t color, uint16_t bg, uint8_t size) {
    if (ch < 32 || ch > 127) return;
    
    uint8_t charIndex = ch - 32;
    
    for (uint8_t col = 0; col < 5; col++) {
        uint8_t line = font5x7[charIndex][col];
        
        for (uint8_t row = 0; row < 8; row++) {
            bool pixelOn = (line & (1 << row)) != 0;
            uint16_t pixelColor = pixelOn ? color : bg;
            
            if (size == 1) {
                TFT_DrawPixel(x + col, y + row, pixelColor);
            } else {
                TFT_FillRect(x + col * size, y + row * size, size, size, pixelColor);
            }
        }
    }
    
    // Draw spacing column
    for (uint8_t row = 0; row < 8; row++) {
        if (size == 1) {
            TFT_DrawPixel(x + 5, y + row, bg);
        } else {
            TFT_FillRect(x + 5 * size, y + row * size, size, size, bg);
        }
    }
}

void TFT_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t size) {
    uint16_t cursorX = x;
    
    while (*str) {
        if (*str == '\n') {
            cursorX = x;
            y += 8 * size;
        } else if (*str == '\r') {
            // Ignore carriage return
        } else {
            TFT_DrawChar(cursorX, y, *str, color, bg, size);
            cursorX += 6 * size;
            
            if (cursorX + 6 * size > SCREEN_WIDTH) {
                cursorX = x;
                y += 8 * size;
            }
        }
        str++;
    }
}

/* ========== Main Program ========== */

/*
int main(void) {
    // Initialize system configuration from SysConfig
    SYSCFG_DL_init();
    
    // Set initial GPIO states
    CS_High();
    DC_Data();
    DL_GPIO_setPins(TFT_RESET_PORT, TFT_RESET_PIN);
    
    // Configure SPI clock divider (~4MHz)
    DL_SPI_setBitRateSerialClockDivider(TFT_SPI_INST, 7);
    DL_SPI_enable(TFT_SPI_INST);
    
    // Initialize TFT display
    TFT_Init();
    
    // Test 1: Verify SPI communication by reading display ID
    uint32_t displayID = TFT_ReadID(); 
    // printf("ID : %d", &displayID);
    bool idCorrect = (displayID == 0x9341);
    
    // Test 2: Write and read back MADCTL register
    TFT_WriteCommand(CMD_MADCTL);
    TFT_WriteData(0x48);
    uint8_t madctlVal = TFT_ReadRegister(CMD_MADCTL);
    bool madctlCorrect = (madctlVal == 0x48);
    
    // Clear screen to black
    TFT_FillScreen(COLOR_BLACK);
    
    // Draw "Hello world" text
    TFT_DrawString(10, 100, "Hello world", COLOR_WHITE, COLOR_BLACK, 2);
    
    // Additional test displays
    TFT_DrawString(10, 130, "MSPM33 TFT Test", COLOR_CYAN, COLOR_BLACK, 1);
    TFT_DrawString(10, 145, "ILI9341 Driver", COLOR_GREEN, COLOR_BLACK, 1);
    
    // Indicate test results via LED
    bool allTestsPassed = idCorrect && madctlCorrect;
    
    if (allTestsPassed) {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
    } else {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);
    }
    
    // Main loop
    while (1) {
        __WFI(); // Wait for interrupt (low power mode)
    }
    
    return 0;
}
*/

/* ========== Main Program ========== */

int main(void) {
    // Initialize system configuration from SysConfig
    SYSCFG_DL_init();
    
    // Set initial GPIO states
    CS_High();
    DC_Data();
    DL_GPIO_setPins(TFT_RESET_PORT, TFT_RESET_PIN);
    
    // Configure SPI clock divider (~4MHz)
    DL_SPI_setBitRateSerialClockDivider(TFT_SPI_INST, 7);
    DL_SPI_enable(TFT_SPI_INST);
    
    // Initialize TFT display
    TFT_Init();
    
    // Clear screen to black
    TFT_FillScreen(COLOR_BLACK);
    
    // Display header
    TFT_DrawString(10, 20, "MSPM33 TFT Test", COLOR_CYAN, COLOR_BLACK, 2);
    TFT_DrawString(10, 50, "ILI9341 Driver", COLOR_GREEN, COLOR_BLACK, 1);
    TFT_DrawString(10, 70, "===================", COLOR_WHITE, COLOR_BLACK, 1);
    
    // Test 1: Verify SPI communication by reading display ID
    TFT_DrawString(10, 90, "Display ID:", COLOR_YELLOW, COLOR_BLACK, 1);
    uint32_t displayID = TFT_ReadID();
    
    // Convert display ID to hex string and display
    char idStr[20];
    sprintf(idStr, "0x%04X", displayID);
    TFT_DrawString(10, 105, idStr, COLOR_WHITE, COLOR_BLACK, 2);
    
    bool idCorrect = (displayID == 0x9341);
    
    // Test 2: Write and read back MADCTL register
    TFT_DrawString(10, 140, "MADCTL Register:", COLOR_YELLOW, COLOR_BLACK, 1);
    TFT_WriteCommand(CMD_MADCTL);
    TFT_WriteData(0x48);
    uint8_t madctlVal = TFT_ReadRegister(CMD_MADCTL);
    
    // Convert MADCTL value to hex string and display
    char madctlStr[20];
    sprintf(madctlStr, "0x%02X", madctlVal);
    TFT_DrawString(10, 155, madctlStr, COLOR_WHITE, COLOR_BLACK, 2);
    
    bool madctlCorrect = (madctlVal == 0x48);
    
    // Display "Hello world" message
    TFT_DrawString(10, 200, "Hello world", COLOR_WHITE, COLOR_BLACK, 2);
    
    // Indicate test results via LED
    bool allTestsPassed = idCorrect && madctlCorrect;
    
    if (allTestsPassed) {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
    } else {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);
    }
    
    // Main loop
    while (1) {
        __WFI(); // Wait for interrupt (low power mode)
    }
    
    return 0;
}