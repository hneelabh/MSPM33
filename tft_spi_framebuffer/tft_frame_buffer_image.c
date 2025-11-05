/*
 * TFT ILI9341 Image Display Code for MSPM33C321x-Q1
 * Optimized for displaying RE Logo from re_logo.h
 */

#include "ti_msp_dl_config.h"
#include <ti/devices/msp/msp.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "re_logo.h"

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

/* ========== Display Dimensions ========== */
#define SCREEN_WIDTH        320
#define SCREEN_HEIGHT       240

/* ========== Color Definitions ========== */
#define COLOR_BLACK         0x0000
#define COLOR_WHITE         0xFFFF
#define COLOR_RED           0xF800
#define COLOR_GREEN         0x07E0
#define COLOR_BLUE          0x001F

/* ========== Frame Buffer ========== */
static uint16_t frameBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

/* ========== Function Prototypes ========== */
static inline void SPI_WriteByte(uint8_t data);
static inline void CS_Low(void);
static inline void CS_High(void);
static inline void DC_Command(void);
static inline void DC_Data(void);
static void TFT_WriteCommand(uint8_t cmd);
static void TFT_WriteData(uint8_t data);
static void TFT_WriteDataBulk(uint8_t *data, uint32_t len);
static void TFT_HardwareReset(void);
static void TFT_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void TFT_Init(void);
uint16_t* TFT_getFrameBuffer(void);
void TFT_clearScreen(uint16_t color);
void TFT_updateScreen(void);
void TFT_drawPixel(uint16_t x, uint16_t y, uint16_t color);
void TFT_fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void TFT_drawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *image);
void TFT_drawImageCentered(uint16_t w, uint16_t h, const uint16_t *image);

/* ========== Low-Level SPI Functions ========== */

static inline void SPI_WriteByte(uint8_t data) {
    DL_SPI_transmitData8(TFT_SPI_INST, data);
    while (DL_SPI_isBusy(TFT_SPI_INST));
}

static inline void CS_Low(void) {
    DL_GPIO_clearPins(TFT_CS_PORT, TFT_CS_PIN);
}

static inline void CS_High(void) {
    DL_GPIO_setPins(TFT_CS_PORT, TFT_CS_PIN);
}

static inline void DC_Command(void) {
    DL_GPIO_clearPins(TFT_DC_PORT, TFT_DC_PIN);
}

static inline void DC_Data(void) {
    DL_GPIO_setPins(TFT_DC_PORT, TFT_DC_PIN);
}

/* ========== TFT Command/Data Functions ========== */

static void TFT_WriteCommand(uint8_t cmd) {
    DC_Command();
    CS_Low();
    SPI_WriteByte(cmd);
    CS_High();
}

static void TFT_WriteData(uint8_t data) {
    DC_Data();
    CS_Low();
    SPI_WriteByte(data);
    CS_High();
}

static void TFT_WriteDataBulk(uint8_t *data, uint32_t len) {
    DC_Data();
    CS_Low();
    for (uint32_t i = 0; i < len; i++) {
        SPI_WriteByte(data[i]);
    }
    CS_High();
}

static void TFT_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    uint8_t xData[4] = {x0 >> 8, x0 & 0xFF, x1 >> 8, x1 & 0xFF};
    uint8_t yData[4] = {y0 >> 8, y0 & 0xFF, y1 >> 8, y1 & 0xFF};

    TFT_WriteCommand(CMD_CASET);
    TFT_WriteDataBulk(xData, 4);
    
    TFT_WriteCommand(CMD_PASET);
    TFT_WriteDataBulk(yData, 4);
    
    TFT_WriteCommand(CMD_RAMWR);
}

/* ========== TFT Initialization ========== */

static void TFT_HardwareReset(void) {
    DL_GPIO_clearPins(TFT_RESET_PORT, TFT_RESET_PIN);
    delay_cycles(32000 * 10);
    DL_GPIO_setPins(TFT_RESET_PORT, TFT_RESET_PIN);
    delay_cycles(32000 * 120);
}

void TFT_updateScreen(void) {
    TFT_SetAddressWindow(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
    
    DC_Data();
    CS_Low();
    
    for (uint32_t i = 0; i < (uint32_t)SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        uint16_t pixel = frameBuffer[i];
        SPI_WriteByte(pixel >> 8);
        SPI_WriteByte(pixel & 0xFF);
    }
    
    CS_High();
}

void TFT_Init(void) {
    TFT_HardwareReset();
    
    TFT_WriteCommand(CMD_SWRESET);
    delay_cycles(32000 * 150);
    
    TFT_WriteCommand(0xCB);
    TFT_WriteData(0x39);
    TFT_WriteData(0x2C);
    TFT_WriteData(0x00);
    TFT_WriteData(0x34);
    TFT_WriteData(0x02);
    
    TFT_WriteCommand(0xCF);
    TFT_WriteData(0x00);
    TFT_WriteData(0xC1);
    TFT_WriteData(0x30);
    
    TFT_WriteCommand(0xE8);
    TFT_WriteData(0x85);
    TFT_WriteData(0x00);
    TFT_WriteData(0x78);
    
    TFT_WriteCommand(0xEA);
    TFT_WriteData(0x00);
    TFT_WriteData(0x00);
    
    TFT_WriteCommand(0xED);
    TFT_WriteData(0x64);
    TFT_WriteData(0x03);
    TFT_WriteData(0x12);
    TFT_WriteData(0x81);
    
    TFT_WriteCommand(0xF7);
    TFT_WriteData(0x20);
    
    TFT_WriteCommand(CMD_PWCTR1);
    TFT_WriteData(0x23);
    
    TFT_WriteCommand(CMD_PWCTR2);
    TFT_WriteData(0x10);
    
    TFT_WriteCommand(CMD_VMCTR1);
    TFT_WriteData(0x3E);
    TFT_WriteData(0x28);
    
    TFT_WriteCommand(CMD_VMCTR2);
    TFT_WriteData(0x86);
    
    TFT_WriteCommand(CMD_MADCTL);
    TFT_WriteData(0x40);
    
    TFT_WriteCommand(CMD_PIXFMT);
    TFT_WriteData(0x55);
    
    TFT_WriteCommand(CMD_FRMCTR1);
    TFT_WriteData(0x00);
    TFT_WriteData(0x18);
    
    TFT_WriteCommand(CMD_DFUNCTR);
    TFT_WriteData(0x08);
    TFT_WriteData(0x82);
    TFT_WriteData(0x27);
    
    TFT_WriteCommand(0xF2);
    TFT_WriteData(0x00);
    
    TFT_WriteCommand(0x26);
    TFT_WriteData(0x01);
    
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
    
    TFT_WriteCommand(CMD_SLPOUT);
    delay_cycles(32000 * 120);
    
    TFT_WriteCommand(CMD_DISPON);
    delay_cycles(32000 * 120);
    
    memset(frameBuffer, COLOR_BLACK, sizeof(frameBuffer));
    TFT_updateScreen();
}

/* ========== Frame Buffer Functions ========== */

uint16_t* TFT_getFrameBuffer(void) {
    return frameBuffer;
}

void TFT_clearScreen(uint16_t color) {
    for (uint32_t i = 0; i < (uint32_t)SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        frameBuffer[i] = color;
    }
    TFT_updateScreen();
}

void TFT_drawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) return;
    frameBuffer[y * SCREEN_WIDTH + x] = color;
}

void TFT_fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT || w == 0 || h == 0) return;
    if (x + w > SCREEN_WIDTH) w = SCREEN_WIDTH - x;
    if (y + h > SCREEN_HEIGHT) h = SCREEN_HEIGHT - y;
    
    for (uint16_t j = y; j < y + h; j++) {
        for (uint16_t i = x; i < x + w; i++) {
            frameBuffer[j * SCREEN_WIDTH + i] = color;
        }
    }
}

/* ========== IMAGE DRAWING FUNCTIONS ========== */

void TFT_drawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *image) {
    if (image == NULL) return;
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) return;
    
    uint16_t drawW = w;
    uint16_t drawH = h;
    
    if (x + w > SCREEN_WIDTH) drawW = SCREEN_WIDTH - x;
    if (y + h > SCREEN_HEIGHT) drawH = SCREEN_HEIGHT - y;
    
    for (uint16_t j = 0; j < drawH; j++) {
        uint16_t py = y + j;
        uint32_t fbOffset = py * SCREEN_WIDTH + x;
        uint32_t imgOffset = j * w;
        
        for (uint16_t i = 0; i < drawW; i++) {
            frameBuffer[fbOffset + i] = image[imgOffset + i];
        }
    }
}

void TFT_drawImageCentered(uint16_t w, uint16_t h, const uint16_t *image) {
    uint16_t x = (SCREEN_WIDTH - w) / 2;
    uint16_t y = (SCREEN_HEIGHT - h) / 2;
    TFT_drawImage(x, y, w, h, image);
}



int main(void) {
    SYSCFG_DL_init();
    
    CS_High();
    DC_Data();
    DL_GPIO_setPins(TFT_RESET_PORT, TFT_RESET_PIN);
    
    DL_SPI_setBitRateSerialClockDivider(TFT_SPI_INST, 7);
    DL_SPI_enable(TFT_SPI_INST);
    
    TFT_Init();
    
    TFT_clearScreen(COLOR_WHITE);
    
    TFT_drawImageCentered(RE_LOGO_WIDTH, RE_LOGO_HEIGHT, reLogo);
    
    TFT_updateScreen();
    
    while (1) {
        __WFI();
    }
    
    return 0;
}

/* ========== MAIN PROGRAM ========== 
int main(void) {
    SYSCFG_DL_init();
   
    CS_High();
    DC_Data();
    DL_GPIO_setPins(TFT_RESET_PORT, TFT_RESET_PIN);
   
    DL_SPI_setBitRateSerialClockDivider(TFT_SPI_INST, 7);
    DL_SPI_enable(TFT_SPI_INST);
   
    TFT_Init();  // Already clears and updates to black
   
    TFT_drawImageCentered(RE_LOGO_WIDTH, RE_LOGO_HEIGHT, reLogo);
    TFT_updateScreen();
   
    while (1) {
        __WFI();
    }
   
    return 0;
}
*/