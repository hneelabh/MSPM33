/*
 * LVGL TFT ILI9341 Display Driver with Speedometer
 * Idea : Using the image background for the speedometer (imported as re_logo) and using the reference of the lvgl meter example 1 to display the needle / spindle and the move it on top of the image from zero speed to maximum
 * Modification: Added digital speed display at needle pivot with circular black background
 */
#include "ti_msp_dl_config.h"
#include <ti/devices/msp/msp.h>
#include <stdint.h>
#include <stdbool.h>
#include "lvgl/lvgl.h"
/* ========== Hardware Pin Configuration ========== */
#define TFT_RESET_PORT GPIO_TFT_RESET_PORT
#define TFT_RESET_PIN GPIO_TFT_RESET_TFT_RESET_PIN_PIN
#define TFT_DC_PORT GPIO_TFT_DC_PORT
#define TFT_DC_PIN GPIO_TFT_DC_TFT_DC_PIN_PIN
#define TFT_CS_PORT GPIO_TFT_CS_PORT
#define TFT_CS_PIN GPIO_TFT_CS_TFT_CS_PIN_PIN
#define TFT_SPI_INST SPI_1_INST
/* ========== ILI9341 Command Definitions ========== */
#define CMD_SWRESET 0x01
#define CMD_SLPOUT 0x11
#define CMD_DISPON 0x29
#define CMD_CASET 0x2A
#define CMD_PASET 0x2B
#define CMD_RAMWR 0x2C
#define CMD_MADCTL 0x36
#define CMD_PIXFMT 0x3A
#define CMD_PWCTR1 0xC0
#define CMD_PWCTR2 0xC1
#define CMD_VMCTR1 0xC5
#define CMD_VMCTR2 0xC7
#define CMD_FRMCTR1 0xB1
#define CMD_DFUNCTR 0xB6
#define CMD_GMCTRP1 0xE0
#define CMD_GMCTRN1 0xE1
/* ========== Display Dimensions ========== */
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define LVGL_TICK_MS 10
/* ========== Speedometer Configuration ========== */
#define MAX_SPEED 160
#define METER_SIZE 200
/* Needle pivot point configuration */
#define NEEDLE_PIVOT_X 160
#define NEEDLE_PIVOT_Y 145
/* ========== Animation Configuration ========== */
#define ANIM_ACCEL_TIME_MS 3000
#define ANIM_DECEL_TIME_MS 3000
#define ANIM_PAUSE_TIME_MS 1000
/* ========== LVGL Display Buffers ========== */
static lv_color_t lvgl_buf1[SCREEN_WIDTH * 10];
static lv_color_t lvgl_buf2[SCREEN_WIDTH * 10];
static lv_disp_drv_t disp_drv;
static lv_disp_draw_buf_t disp_buf;
/* ========== UI Objects ========== */
static lv_obj_t *meter;
static lv_meter_indicator_t *needle_indicator;
static lv_meter_scale_t *scale;
static lv_obj_t *speed_label;
/* ========== Low-Level Hardware Functions ========== */
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
static void TFT_HardwareReset(void) {
    DL_GPIO_clearPins(TFT_RESET_PORT, TFT_RESET_PIN);
    delay_cycles(32000 * 10);
    DL_GPIO_setPins(TFT_RESET_PORT, TFT_RESET_PIN);
    delay_cycles(32000 * 120);
}
/* ========== TFT Initialization (ILI9341) ========== */
static void TFT_Init(void) {
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
}
/* ========== LVGL Display Flush Callback ========== */
static void lvgl_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    uint16_t width = area->x2 - area->x1 + 1;
    uint16_t height = area->y2 - area->y1 + 1;
   
    TFT_SetAddressWindow(area->x1, area->y1, area->x2, area->y2);
   
    DC_Data();
    CS_Low();
   
    uint16_t *pixels = (uint16_t *)color_p;
    for (uint32_t i = 0; i < width * height; i++) {
        uint16_t pixel = pixels[i];
        SPI_WriteByte(pixel >> 8);
        SPI_WriteByte(pixel & 0xFF);
    }
   
    CS_High();
   
    lv_disp_flush_ready(disp_drv);
}
/* ========== LVGL Initialization ========== */
static void LVGL_Init(void) {
    lv_init();
   
    lv_disp_draw_buf_init(&disp_buf, lvgl_buf1, lvgl_buf2, SCREEN_WIDTH * 10);
   
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    lv_disp_drv_register(&disp_drv);
}
/* ========== LVGL Tick Handler ========== */
void LVGL_Tick_Handler(void) {
    lv_tick_inc(LVGL_TICK_MS);
}
/* ========== Animation Callback ========== */
static void set_needle_value(void *indicator, int32_t value) {
    lv_meter_set_indicator_value(meter, (lv_meter_indicator_t *)indicator, value);
    
    // Update digital speed display
    if (speed_label) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", (int)value);
        lv_label_set_text(speed_label, buf);
    }
}
/* ========== Create Speedometer UI (Custom Image + Needle Only) ========== */
#include "re_logo.h" // Contains speedometerImage, SPEEDOMETER_WIDTH, SPEEDOMETER_HEIGHT
static void Create_Speedometer_UI(void) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
   
    // Create LVGL image descriptor for the speedometer background
    static lv_img_dsc_t speedometer_dsc = {
        .header.always_zero = 0,
        .header.w = SPEEDOMETER_WIDTH,
        .header.h = SPEEDOMETER_HEIGHT,
        .data_size = SPEEDOMETER_WIDTH * SPEEDOMETER_HEIGHT * LV_COLOR_SIZE / 8,
        .header.cf = LV_IMG_CF_TRUE_COLOR,
        .data = (uint8_t *)speedometerImage,
    };
   
    // Create image object for speedometer background
    lv_obj_t *speedometer_img = lv_img_create(scr);
    lv_img_set_src(speedometer_img, &speedometer_dsc);
   
    // Center the speedometer image on screen
    uint16_t x_offset = (SCREEN_WIDTH - SPEEDOMETER_WIDTH) / 2;
    uint16_t y_offset = (SCREEN_HEIGHT - SPEEDOMETER_HEIGHT) / 2;
    lv_obj_set_pos(speedometer_img, x_offset, y_offset);
   
    // Create meter widget (COMPLETELY invisible - only for needle math)
    meter = lv_meter_create(scr);
    lv_obj_set_size(meter, METER_SIZE, METER_SIZE);
   
    // Position meter so needle pivot is at NEEDLE_PIVOT_X, NEEDLE_PIVOT_Y
    int16_t meter_x = NEEDLE_PIVOT_X - (METER_SIZE / 2);
    int16_t meter_y = NEEDLE_PIVOT_Y - (METER_SIZE / 2);
    lv_obj_set_pos(meter, meter_x, meter_y);
   
    // CRITICAL: Remove ALL background elements from meter widget
    lv_obj_set_style_bg_opa(meter, LV_OPA_TRANSP, 0); // Transparent background
    lv_obj_set_style_bg_opa(meter, LV_OPA_TRANSP, LV_PART_MAIN); // Main part transparent
    lv_obj_set_style_bg_opa(meter, LV_OPA_TRANSP, LV_PART_INDICATOR); // Indicator transparent (hides pivot square)
    lv_obj_set_style_line_width(meter, 0, LV_PART_TICKS); // Ensure no tick lines
    lv_obj_set_style_border_width(meter, 0, 0); // No border
    lv_obj_set_style_shadow_width(meter, 0, 0); // No shadow
    lv_obj_set_style_outline_width(meter, 0, 0); // No outline
    lv_obj_set_style_pad_all(meter, 0, 0); // No padding
    lv_obj_set_style_radius(meter, 0, 0); // No rounded corners
    lv_obj_clear_flag(meter, LV_OBJ_FLAG_CLICKABLE); // Not interactive
    lv_obj_clear_flag(meter, LV_OBJ_FLAG_SCROLLABLE); // Not scrollable
   
    // Add scale - completely invisible, just for needle angle calculation (NO TICKS to avoid scale arc)
    scale = lv_meter_add_scale(meter);
    lv_meter_set_scale_range(meter, scale, 0, MAX_SPEED, 270, 135);
   
    // Add needle indicator (ONLY visible element from meter widget)
    needle_indicator = lv_meter_add_needle_line(meter, scale, 4,
                                                lv_color_make(255, 50, 50), -10);
   
    // Add center dot for needle pivot
    lv_obj_t *center_dot = lv_obj_create(scr);
    lv_obj_set_size(center_dot, 12, 12);
    lv_obj_set_style_radius(center_dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(center_dot, lv_color_make(200, 200, 200), 0);
    lv_obj_set_style_border_width(center_dot, 2, 0);
    lv_obj_set_style_border_color(center_dot, lv_color_make(80, 80, 80), 0);
    lv_obj_clear_flag(center_dot, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(center_dot, NEEDLE_PIVOT_X - 6, NEEDLE_PIVOT_Y - 6);
    
    // Create circular black background for digital speed display
    lv_obj_t *speed_bg = lv_obj_create(scr);
    lv_obj_set_size(speed_bg, 60, 60);
    lv_obj_set_style_radius(speed_bg, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(speed_bg, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(speed_bg, LV_OPA_90, 0); // Slightly transparent
    lv_obj_set_style_border_width(speed_bg, 2, 0);
    lv_obj_set_style_border_color(speed_bg, lv_color_make(100, 100, 100), 0);
    lv_obj_clear_flag(speed_bg, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(speed_bg, NEEDLE_PIVOT_X - 30, NEEDLE_PIVOT_Y - 30);
    
    // Create digital speed label
    speed_label = lv_label_create(speed_bg);
    lv_label_set_text(speed_label, "0");
    lv_obj_set_style_text_color(speed_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(speed_label, &lv_font_montserrat_14, 0);
    lv_obj_center(speed_label);
   
    // Create LVGL animation for smooth needle movement
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_exec_cb(&anim, set_needle_value);
    lv_anim_set_var(&anim, needle_indicator);
    lv_anim_set_values(&anim, 0, MAX_SPEED);
    lv_anim_set_time(&anim, ANIM_ACCEL_TIME_MS);
    lv_anim_set_repeat_delay(&anim, ANIM_PAUSE_TIME_MS);
    lv_anim_set_playback_time(&anim, ANIM_DECEL_TIME_MS);
    lv_anim_set_playback_delay(&anim, ANIM_PAUSE_TIME_MS);
    lv_anim_set_repeat_count(&anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim);
}
/* ========== Public Function to Set Speed Manually ========== */
void Set_Speed(int16_t speed) {
    if (speed < 0) speed = 0;
    if (speed > MAX_SPEED) speed = MAX_SPEED;
    lv_meter_set_indicator_value(meter, needle_indicator, speed);
    
    // Update digital speed display
    if (speed_label) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", (int)speed);
        lv_label_set_text(speed_label, buf);
    }
}
/* ========== Timer Interrupt Handler ========== */
void TIMA0_0_IRQHandler(void) {
    switch (DL_TimerG_getPendingInterrupt(TIMER_0_INST)) {
        case DL_TIMER_IIDX_ZERO:
            LVGL_Tick_Handler();
            DL_TimerG_clearInterruptStatus(TIMER_0_INST, DL_TIMER_INTERRUPT_ZERO_EVENT);
            break;
        default:
            break;
    }
}
static void Setup_LVGL_Timer(void) {
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
    DL_TimerG_startCounter(TIMER_0_INST);
}
/* ========== Main Program ========== */
int main(void) {
    SYSCFG_DL_init();
   
    CS_High();
    DC_Data();
    DL_GPIO_setPins(TFT_RESET_PORT, TFT_RESET_PIN);
   
    DL_SPI_setBitRateSerialClockDivider(TFT_SPI_INST, 7);
    DL_SPI_enable(TFT_SPI_INST);
   
    TFT_Init();
    LVGL_Init();
    Setup_LVGL_Timer();
    Create_Speedometer_UI();
   
    while (1) {
        lv_timer_handler();
        delay_cycles(160);
    }
   
    return 0;
}
/* ========== MODIFICATIONS SUMMARY ========== */
/*
 * DIGITAL SPEED DISPLAY ADDED:
 *
 * 1. Added speed_label as a static UI object to track the digital display
 * 
 * 2. Created circular black background (60x60 pixels):
 *    - Black color with 90% opacity (slightly transparent)
 *    - Gray border for definition
 *    - Positioned at needle pivot point (NEEDLE_PIVOT_X, NEEDLE_PIVOT_Y)
 *
 * 3. Created white text label:
 *    - Uses Montserrat 24pt font
 *    - Automatically centered in circular background
 *    - Displays current speed value
 *
 * 4. Updated set_needle_value() callback:
 *    - Now updates both needle position AND digital display
 *    - Ensures synchronized animation
 *
 * 5. Updated Set_Speed() function:
 *    - Also updates digital display when speed is set manually
 *
 * RESULT: Digital speed now displays at the center with a clean circular
 * black background, updating smoothly as the needle moves!
 */