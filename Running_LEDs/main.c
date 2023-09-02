/* Defines boolean and integer data types */
#include <stdbool.h>
#include <stdint.h>
#include "tm4c129encpdt.h"

#define delay_value 50000

int main(void)
{

    /* Delay Loop variable */
    volatile unsigned long ulLoop;

    /* Enable the GPIO port that is used for the onboard Switch */
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;

    while(!(SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R5)){}; // check if the peripheral register is ready for GPIO(PRGPIO)

    // Enabling PORTN clock
    SYSCTL_RCGCGPIO_R |=SYSCTL_RCGCGPIO_R12 ;

    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R12)){};// check if peripheral is ready


    // Setting PORTF Pin 1 and 0 as Output
    GPIO_PORTF_AHB_DIR_R |= 1UL << 4UL | 1UL << 0UL;

    // Enabling digital for PORTJ.
    GPIO_PORTF_AHB_DEN_R |= (1UL << 4UL | 1UL << 0UL);

    //No alternate function sellect
    GPIO_PORTF_AHB_AFSEL_R = 0x00;



    // Set portN pin 1 AND O as output pin
    GPIO_PORTN_DIR_R |= (1UL << 1UL | 1UL << 0UL);

    // Enabling digital for PORTN pin 1 and 0.
    GPIO_PORTN_DEN_R = (1UL << 1UL | 1UL << 0UL);

    // No alternate function select.
    GPIO_PORTN_AFSEL_R = 0x00;




    while (1)
    {
       GPIO_PORTN_DATA_R |= 1UL << 1UL;
       for(ulLoop = 0; ulLoop < delay_value; ulLoop++);
       GPIO_PORTN_DATA_R &= ~(1UL << 1UL);
       for(ulLoop = 0; ulLoop < delay_value; ulLoop++);

       GPIO_PORTN_DATA_R |= 1UL << 0UL;
       for(ulLoop = 0; ulLoop < delay_value; ulLoop++);
       GPIO_PORTN_DATA_R &= ~(1UL << 0UL);
       for(ulLoop = 0; ulLoop < delay_value; ulLoop++);


       GPIO_PORTF_AHB_DATA_R |= 1UL << 4UL;
       for(ulLoop = 0; ulLoop < delay_value; ulLoop++);
       GPIO_PORTF_AHB_DATA_R &= ~(1UL << 4UL);
       for(ulLoop = 0; ulLoop < delay_value; ulLoop++);


       GPIO_PORTF_AHB_DATA_R |= 1UL << 0UL;
       for(ulLoop = 0; ulLoop < delay_value; ulLoop++);
       GPIO_PORTF_AHB_DATA_R &= ~(1UL << 0UL);
       for(ulLoop = 0; ulLoop < delay_value; ulLoop++);

       GPIO_PORTF_AHB_DATA_R |= 1UL << 4UL;
       for(ulLoop = 0; ulLoop < delay_value; ulLoop++);
       GPIO_PORTF_AHB_DATA_R &= ~(1UL << 4UL);
       for(ulLoop = 0; ulLoop < delay_value; ulLoop++);

       GPIO_PORTN_DATA_R |= 1UL << 0UL;
       for(ulLoop = 0; ulLoop < delay_value; ulLoop++);
       GPIO_PORTN_DATA_R &= ~(1UL << 0UL);
       for(ulLoop = 0; ulLoop < delay_value; ulLoop++);

    }
}

