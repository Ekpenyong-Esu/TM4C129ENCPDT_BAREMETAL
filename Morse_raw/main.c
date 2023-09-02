/* Defines boolean and integer data types */
#include <stdbool.h>
#include <stdint.h>
#include "tm4c129encpdt.h"



#define BUZZER (1UL << 1UL)


void dash();
void dot();
void delay_ms(int ms);


int main(void)
{

    /* Enable the GPIO port that is used for the onboard Switch */
    SYSCTL_RCGCGPIO_R |=SYSCTL_RCGCGPIO_R8  ;

    while(!(SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R8 )){}; // check if the peripheral register is ready for GPIO(PRGPIO)

    // Enabling PORTF clock
    SYSCTL_RCGCGPIO_R |=SYSCTL_RCGCGPIO_R5 ;

    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5)){};// check if peripheral is ready


    // Setting PORTJ Pin 1 and 0 as input
    GPIO_PORTJ_AHB_DIR_R &= ~(1UL << 1UL | 1UL << 0UL);

    // Enabling digital for PORTJ.
    GPIO_PORTJ_AHB_DEN_R |= (1UL << 1UL | 1UL << 0UL);



    // Set portF pin 1 AND O as output pin
    GPIO_PORTF_AHB_DIR_R  |= (1UL << 2UL | 1UL << 1UL);

    // Enabling digital for PORTN pin 1 and 0.
    GPIO_PORTF_AHB_DEN_R  = (1UL << 2UL | 1UL << 1UL);

    // No alternate function select.
    GPIO_PORTF_AHB_AFSEL_R= 0x00;
    GPIO_PORTJ_AHB_AFSEL_R= 0x00;


    // PULL up the the switches in PORTJ pin 1 and 0
    GPIO_PORTJ_AHB_PUR_R |= (1UL << 1UL | 1UL << 0UL);

    while (1)
    {
        if ((GPIO_PORTJ_AHB_DATA_R & (1UL << 0)) != 0x1) {
            dot();
        }

        if ((GPIO_PORTJ_AHB_DATA_R & (1UL << 1UL)) != 0x2) {
            dash();
        }
    }
}



// Generate a dot
void dot() {
  GPIO_PORTF_AHB_DATA_R |= BUZZER;
  delay_ms(100);
  GPIO_PORTF_AHB_DATA_R &= ~BUZZER;
  delay_ms(50);
}


// Generate a dash
void dash() {
  GPIO_PORTF_AHB_DATA_R |= BUZZER;
  delay_ms(300);
  GPIO_PORTF_AHB_DATA_R &= ~BUZZER;
  delay_ms(50);
}

// Software delay
void delay_ms(int ms) {
  int i;
  for(i=0; i<ms*3000; i++);
}
