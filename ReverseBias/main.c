/* Defines boolean and integer data types */
#include <stdbool.h>
#include <stdint.h>
#include "tm4c129encpdt.h"


void delay(uint32_t count)
{
    uint32_t i;
    volatile uint32_t j;
    for (i = 0; i < count; i++)
    {
        for (j = 0; j < 1000; j++)
        {
        } // Delay loop
    }
}


int main(void) {
    SYSCTL_RCGCGPIO_R |= 1UL << 5UL;  // Enable clock for GPIO Port F

    GPIO_PORTF_AHB_DIR_R |= (1UL << 4UL) | (1UL << 0UL);     // Set PD4 and PD0 as output
    GPIO_PORTF_AHB_DEN_R |= (1UL << 4UL) | (1UL << 0UL);     // Enable digital function for PD0 and PD2

    while (1) {
        GPIO_PORTF_AHB_DATA_R |=  (1UL << 0UL); // Set PD0 High (reverse bias mode)
        GPIO_PORTF_AHB_DATA_R &= ~(1UL << 4UL); // Set PD2 Low

        GPIO_PORTF_AHB_DIR_R &= ~(1UL << 0UL); // Set PD0 as input

        uint32_t counter = 0;

        while ((GPIO_PORTF_AHB_DATA_R & 0x01) != 0) {
            counter++;              // Count until PD0 goes to logic low (light intensity)
        }

        GPIO_PORTF_AHB_DIR_R |=  1UL << 0UL;      // Set PD0 back to output
        GPIO_PORTF_AHB_DATA_R &= ~(1UL << 0UL);     // Set PD0 Low
        GPIO_PORTF_AHB_DATA_R |= 1UL << 4UL;      // Set PD2 High (forward bias mode, LED on)

        delay(counter);                       // Delay proportional to the counter value

        GPIO_PORTF_AHB_DATA_R &= ~(1UL << 4UL | 1UL << 0UL); // Set PD0 and PD2 Low (LED off)

        delay(counter);                      // Delay proportional to the counter value
    }
}
