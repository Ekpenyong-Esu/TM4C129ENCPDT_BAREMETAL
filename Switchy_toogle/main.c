/* Defines boolean and integer data types */
#include <stdbool.h>
#include <stdint.h>
#include "tm4c129encpdt.h"

// Simple delay function
void delay(uint32_t msec) {
  int i;
  for(i = 0; i < msec*3000; i++);
}

int main(void)
{
    uint8_t flag = 1;

    /* Enable the GPIO port that is used for the onboard Switch */
    SYSCTL_RCGCGPIO_R |=SYSCTL_RCGCGPIO_R8  ;

    while(!(SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R8 )){}; // check if the peripheral register is ready for GPIO(PRGPIO)

    // Enabling PORTN clock
    SYSCTL_RCGCGPIO_R |=SYSCTL_RCGCGPIO_R12 ;

    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R12)){};// check if peripheral is ready


    // Setting PORTJ Pin 0 as input
    GPIO_PORTJ_AHB_DIR_R &= ~(1UL << 0UL);

    // Enabling digital for PORTJ.
    GPIO_PORTJ_AHB_DEN_R |= 1UL << 0UL;

    GPIO_PORTJ_AHB_PUR_R |= 1UL << 0U;



    // Set portN pin O as output pin
    GPIO_PORTN_DIR_R |= 1UL << 1UL;

    // Enabling digital function.
    GPIO_PORTN_DEN_R |= 1UL << 1UL;

    // seting alternate function off for both port N and port J
    GPIO_PORTN_AFSEL_R = 0x00;

    GPIO_PORTJ_AHB_AFSEL_R = 0x00;


    /* Loop forever */
    while (1)
    {
        if ((GPIO_PORTJ_AHB_DATA_R & (1UL << 0)) == 0) {

            if (flag){
                /* Turn on the LED */
                GPIO_PORTN_DATA_R |= 1UL << 1UL;
                flag = !flag;
                delay(100);
            }
            else
            {
                /* Turn on the LED */
                GPIO_PORTN_DATA_R &= ~1UL << 1UL;
                flag = !flag;
                delay(100);

            }
        }

    }

}
