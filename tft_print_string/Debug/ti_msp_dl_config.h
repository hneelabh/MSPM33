/*
 * Copyright (c) 2025, Texas Instruments Incorporated - http://www.ti.com
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

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM33 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM33C321X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM33C321X

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m33/dl_core.h>
#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define CPUCLK_FREQ                                                     32000000




/* Port definition for Pin Group GPIO_TFT_CS */
#define GPIO_TFT_CS_PORT                                                 (GPIOC)

/* Defines for TFT_CS_PIN: GPIOC.8 with pinCMx 86 on package pin 80 */
#define GPIO_TFT_CS_TFT_CS_PIN_PIN                               (DL_GPIO_PIN_8)
#define GPIO_TFT_CS_TFT_CS_PIN_IOMUX                             (IOMUX_PINCM86)
/* Port definition for Pin Group GPIO_TFT_DC */
#define GPIO_TFT_DC_PORT                                                 (GPIOC)

/* Defines for TFT_DC_PIN: GPIOC.12 with pinCMx 61 on package pin 10 */
#define GPIO_TFT_DC_TFT_DC_PIN_PIN                              (DL_GPIO_PIN_12)
#define GPIO_TFT_DC_TFT_DC_PIN_IOMUX                             (IOMUX_PINCM61)
/* Port definition for Pin Group GPIO_TFT_RESET */
#define GPIO_TFT_RESET_PORT                                              (GPIOC)

/* Defines for TFT_RESET_PIN: GPIOC.11 with pinCMx 89 on package pin 88 */
#define GPIO_TFT_RESET_TFT_RESET_PIN_PIN                        (DL_GPIO_PIN_11)
#define GPIO_TFT_RESET_TFT_RESET_PIN_IOMUX                       (IOMUX_PINCM89)
/* Port definition for Pin Group GPIO_LEDS */
#define GPIO_LEDS_PORT                                                   (GPIOC)

/* Defines for FAIL_LED: GPIOC.26 with pinCMx 91 on package pin 90 */
#define GPIO_LEDS_FAIL_LED_PIN                                  (DL_GPIO_PIN_26)
#define GPIO_LEDS_FAIL_LED_IOMUX                                 (IOMUX_PINCM91)
/* Defines for PASS_LED: GPIOC.27 with pinCMx 92 on package pin 91 */
#define GPIO_LEDS_PASS_LED_PIN                                  (DL_GPIO_PIN_27)
#define GPIO_LEDS_PASS_LED_IOMUX                                 (IOMUX_PINCM92)


/* Defines for SPI_1 */
#define SPI_1_INST                                                          UC6
#define SPI_1_INST_IRQHandler                                    UC6_IRQHandler
#define SPI_1_INST_INT_IRQN                                        UC6_INT_IRQn
#define GPIO_SPI_1_PICO_PORT                                              GPIOC
#define GPIO_SPI_1_PICO_PIN                                       DL_GPIO_PIN_6
#define GPIO_SPI_1_IOMUX_PICO                                   (IOMUX_PINCM84)
#define GPIO_SPI_1_IOMUX_PICO_FUNC                    IOMUX_PINCM84_PF_UC6_PICO
#define GPIO_SPI_1_POCI_PORT                                              GPIOC
#define GPIO_SPI_1_POCI_PIN                                       DL_GPIO_PIN_9
#define GPIO_SPI_1_IOMUX_POCI                                   (IOMUX_PINCM87)
#define GPIO_SPI_1_IOMUX_POCI_FUNC                    IOMUX_PINCM87_PF_UC6_POCI
/* GPIO configuration for SPI_1 */
#define GPIO_SPI_1_SCLK_PORT                                              GPIOC
#define GPIO_SPI_1_SCLK_PIN                                       DL_GPIO_PIN_7
#define GPIO_SPI_1_IOMUX_SCLK                                   (IOMUX_PINCM85)
#define GPIO_SPI_1_IOMUX_SCLK_FUNC                    IOMUX_PINCM85_PF_UC6_SCLK
#define GPIO_SPI_1_CS0_PORT                                               GPIOA
#define GPIO_SPI_1_CS0_PIN                                       DL_GPIO_PIN_12
#define GPIO_SPI_1_IOMUX_CS0                                    (IOMUX_PINCM34)
#define GPIO_SPI_1_IOMUX_CS0_FUNC                      IOMUX_PINCM34_PF_UC6_CS0

/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);

void SYSCFG_DL_SPI_1_init(void);


#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
