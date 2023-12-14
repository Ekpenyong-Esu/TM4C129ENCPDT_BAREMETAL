/* Defines boolean and integer data types */
#include <stdbool.h>
#include <stdint.h>
#include "tm4c129encpdt.h"


// Connect PJ0 to PD6

#define delay_value 135000



void interruptBlinking(void);

/* Delay Loop variable */
volatile unsigned long ulLoop;

int main(void)
{
    /* Enable the GPIO port that is used for the onboard LED */
    // Enable the clock for GPIOD
    SYSCTL_RCGCGPIO_R |=SYSCTL_RCGCGPIO_R3;

    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R3)){};  // check if the peripheral register is ready for GPIO(PRGPIO)

    /* Enable the GPIO port that is used for the onboard Switch port j */
    SYSCTL_RCGCGPIO_R |=SYSCTL_RCGCGPIO_R8  ;

    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R8 )){}; // check if the peripheral register is ready for GPIO(PRGPIO)


    // Enable the clock for GPION
    SYSCTL_RCGCGPIO_R |=SYSCTL_RCGCGPIO_R12 ;

    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R12)){};


    // Setting PORTJ Pin 1 and 0 as input
    GPIO_PORTJ_AHB_DIR_R &= ~(1UL << 0U);

    // Enabling digital for PORTJ.
    GPIO_PORTJ_AHB_DEN_R |= (1UL << 0U);

    // PULL up the the switches in PORTJ pin 1 and 0
    GPIO_PORTJ_AHB_PUR_R |= (1UL << 0U);


    // Set portN pin 1 AND O as output pin
    GPIO_PORTN_DIR_R |= (1UL << 1UL | 1UL << 0U);

    // Enabling digital function.
    GPIO_PORTN_DEN_R |= (1UL << 1UL | 1UL << 0U);


    // Enable the GPIOD pin 6 for input
    GPIO_PORTD_AHB_DIR_R  &= ~(0x40); // Set pin PD6 for input
    GPIO_PORTD_AHB_DEN_R |= 0x40;  // Enable digital function on P

    // Configure interrupt for P
    GPIO_PORTD_AHB_IS_R &= ~(0x40);   // Set PD6 as edge-sensitive
    GPIO_PORTD_AHB_IBE_R &= 0x40;  // Set PD6 as bothsingle edge-triggered
    GPIO_PORTD_AHB_IEV_R &= ~(0x40);  // Set PD6 to trigger on falling edge

    GPIO_PORTD_AHB_IM_R  |= (0x40);    // Enable interrupt mask for PD6
    GPIO_PORTD_AHB_ICR_R  = 0x40;   // Clear pD6 interrupt

    NVIC_PRI0_R &= ~(0x7 << 29);
    NVIC_PRI0_R |= 0x1 << 29;

    NVIC_EN0_R |= (1U << 3);



    GPIO_PORTN_AFSEL_R = 0x00;

    /* Loop forever */
    while (1)
    {

    }
}

void PortD_IntHandler(void)
{
    interruptBlinking();
    GPIO_PORTD_AHB_ICR_R = 0x40;   // Clear pD6 interrupt

}

void interruptBlinking(void)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        /* Turn on the LED */
        GPIO_PORTN_DATA_R |= 3UL << 0UL;

        /* Delay for a 100ms */
        for (ulLoop = 0; ulLoop < delay_value; ulLoop++){}

        /* Turn off the LED */
        GPIO_PORTN_DATA_R &= ~(3 << 0);

        /* Delay for a 100ms */
        for (ulLoop = 0; ulLoop < delay_value; ulLoop++){}
    }
}

