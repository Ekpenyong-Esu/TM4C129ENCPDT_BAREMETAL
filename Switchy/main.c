/* Defines boolean and integer data types */
#include <stdbool.h>
#include <stdint.h>
#include "tm4c129encpdt.h"



int main(void)
{

    /* Enable the GPIO port that is used for the onboard Switch */
    SYSCTL_RCGCGPIO_R |=SYSCTL_RCGCGPIO_R8  ;

    while(!(SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R8 )){}; // check if the peripheral register is ready for GPIO(PRGPIO)

    // Enabling PORTN clock
    SYSCTL_RCGCGPIO_R |=SYSCTL_RCGCGPIO_R12 ;

    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R12)){};// check if peripheral is ready


    // Setting PORTJ Pin 1 and 0 as input
    GPIO_PORTJ_AHB_DIR_R &= ~(1UL << 1UL | 1UL << 0UL);

    // Enabling digital for PORTJ.
    GPIO_PORTJ_AHB_DEN_R |= (1UL << 1UL | 1UL << 0UL);



    // Set portN pin 1 AND O as output pin
    GPIO_PORTN_DIR_R |= (1UL << 1UL | 1UL << 0UL);

    // Enabling digital for PORTN pin 1 and 0.
    GPIO_PORTN_DEN_R = (1UL << 1UL | 1UL << 0UL);

    // No alternate function select.
    GPIO_PORTN_AFSEL_R = 0x00;


    // PULL up the the switches in PORTJ pin 1 and 0
    GPIO_PORTJ_AHB_PUR_R |= (1UL << 1UL | 1UL << 0UL);

    while (1)
    {
        if ((GPIO_PORTJ_AHB_DATA_R & (1UL << 0)) != 0x1) {
            /* Turn on the LED */
            GPIO_PORTN_DATA_R |= 1UL << 0UL;
        }
        else {
            GPIO_PORTN_DATA_R &= ~(1UL << 0UL);
        }

        if ((GPIO_PORTJ_AHB_DATA_R & (1UL << 1UL)) != 0x2) {
            GPIO_PORTN_DATA_R |= 1UL << 1UL;
        }
        else {
            GPIO_PORTN_DATA_R &= ~(1UL << 1UL);
        }
    }
}

