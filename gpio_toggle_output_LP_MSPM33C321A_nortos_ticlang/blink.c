#include "ti_msp_dl_config.h"

int main(void)
{
    // Initialize the system
    SYSCFG_DL_init();

    // Infinite loop
    while (1) 
    {
        // Turn LED ON
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);
        
        // Wait (delay)
        delay_cycles(16000000);  // ~0.5 second delay at 32MHz
        
        // Turn LED OFF
        DL_GPIO_clearPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);
        
        // Wait (delay)
        delay_cycles(16000000);  // ~0.5 second delay at 32MHz
    }
}