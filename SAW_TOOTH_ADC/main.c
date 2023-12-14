#include <stdint.h>
#include "tm4c129encpdt.h"

void delayMs(uint32_t n);

int main(void)
{
    uint8_t i = 0;

    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;   // Enable clock to GPIO Port B

    // Use Port B for DAC
    GPIO_PORTB_AHB_DIR_R |= 0xFF;             // Set PORTB as output
    GPIO_PORTB_AHB_DEN_R |= 0xFF;             // Enable digital pins

    while (1)
    {
        GPIO_PORTB_AHB_DATA_R = i;           // Send out i
        i++;
        delayMs(1);
    }
}

// Delay n milliseconds (assuming a 120 MHz CPU clock)
void delayMs(uint32_t n)
{
    int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < 3180; j++)
            {} // Do nothing for 1 ms
}
