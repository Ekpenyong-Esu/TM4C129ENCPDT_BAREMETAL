/* Defines boolean and integer data types */
#include <stdbool.h>
#include <stdint.h>
#include "tm4c129encpdt.h"


#define delay_value 135000


int main(void)
{
    /* Delay Loop variable */
    volatile unsigned long ulLoop;

    /* Enable the GPIO port that is used for the onboard LED */
    SYSCTL_RCGCGPIO_R |=SYSCTL_RCGCGPIO_R5 ;

    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5)){};  // check if the peripheral register is ready for GPIO(PRGPIO)


    SYSCTL_RCGCGPIO_R |=SYSCTL_RCGCGPIO_R12 ;

    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R12)){};

    /* Do a dummy read to insert a few cycles after peripheral enable */
    //ulLoop = SYSCTL_RCGCGPIO_R;

    // Set portN pin 1 AND O as output pin
    GPIO_PORTN_DIR_R |= (1UL << 1UL | 1UL << 0UL);

    // Enabling digital function.
    GPIO_PORTN_DEN_R |= (1UL << 1UL | 1UL << 0UL);

    // Set portF pin 4 AND O as output pin
    GPIO_PORTF_AHB_DIR_R  |= (1UL << 4UL |1UL << 0UL);

    // Enabling digital function.
    GPIO_PORTF_AHB_DEN_R |= (1UL << 4UL |1UL << 0UL);

    GPIO_PORTN_AFSEL_R = 0x00;

    GPIO_PORTF_AHB_AFSEL_R = 0x00;
    /* Loop forever */
    while (1)
    {
        /* Turn on the LED */
        GPIO_PORTN_DATA_R |= 3UL << 0UL;
        GPIO_PORTF_AHB_DATA_R |= (1UL << 4UL | 1UL << 0UL);
        /* Delay for a 100ms */
        for (ulLoop = 0; ulLoop < delay_value; ulLoop++)
        {
        }
        /* Turn off the LED */
        GPIO_PORTN_DATA_R &= ~(3 << 0);
        GPIO_PORTF_AHB_DATA_R &= ~(1UL << 4UL | 1UL << 0UL);
        /* Delay for a 100ms */
        for (ulLoop = 0; ulLoop < delay_value; ulLoop++)
        {
        }
    }
}

