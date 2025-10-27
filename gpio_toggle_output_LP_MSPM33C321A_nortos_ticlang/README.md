## Example Summary

This example demonstrates how to toggle GPIO pins using the hardware toggle register. Three GPIO pins connected to the RGB LED on the LaunchPad are configured as digital outputs and toggled in a pattern.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOA | PA2 | Standard Output (LED2 Blue) |
| GPIOC | PC26 | Standard Output (LED2 Red) |
| GPIOC | PC27 | Standard Output (LED2 Green) |
| SYSCTL |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PA2 | GPIOA | USER_LED_1 | N/A | <ul><li>PA2 is connected to LED2 Blue<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC26 | GPIOC | USER_LED_2 | N/A | <ul><li>PC26 is connected to LED2 Red<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC27 | GPIOC | USER_LED_3 | N/A | <ul><li>PC27 is connected to LED2 Green<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging</ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging</ul> |

### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A web page](https://www.ti.com/tool/LP-MSPM33C321A).

## Example Usage

The example performs the following operations:
1. Initializes the GPIO pins as digital outputs
2. Sets the initial state of the LEDs (LED1 and LED3 ON, LED2 OFF)
3. In an infinite loop:
   - Waits for approximately 0.5 seconds
   - Toggles LED1 (Blue) individually
   - Toggles LED2 (Red) and LED3 (Green) together

This creates a pattern where the blue LED toggles separately, while the red and green LEDs toggle together. The hardware toggle register is used to flip the current value of the LEDs without needing additional read-modify-write cycles by the processor.

Compile, load and run the example. The RGB LED will toggle with blue being opposite of red and green.
