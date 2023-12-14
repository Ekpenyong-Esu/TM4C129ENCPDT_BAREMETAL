/* Defines boolean and integer data types */
#include <stdbool.h>
#include <stdint.h>
#include "tm4c129encpdt.h"
//#include "driverlib/interrupt.h" // Include interrupt-related functions

#define delay_value 135000



void interruptSw0Blinking(void);
void interruptSw1Blinking(void);

/* Delay Loop variable */
volatile unsigned long ulLoop;

int main(void)
{
    /* Enable the GPIO port that is used for the onboard LED */
    // Enable the clock for GPIOF
     SYSCTL_RCGCGPIO_R |=SYSCTL_RCGCGPIO_R5 ;

    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5)){};  // check if the peripheral register is ready for GPIO(PRGPIO)

    // Enable the clock for GPION
    SYSCTL_RCGCGPIO_R |=SYSCTL_RCGCGPIO_R12 ;

    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R12)){};

    // Enable the clock for GPIOJ
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R8;

    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R8)){};


    // Enable the GPIO pins for SW1 and SW2 (PJ0 and PJ1)
    GPIO_PORTJ_AHB_DIR_R &= ~0x03; // Set PJ0 and PJ1 as inputs
    GPIO_PORTJ_AHB_DEN_R |= 0x03;  // Enable digital function on PJ0 and PJ1


    // Set SW1 and SW2 as pull-up inputs
    GPIO_PORTJ_AHB_PUR_R |= 0x03;



    // Configure interrupt for SW1 (PJ0)
    GPIO_PORTJ_AHB_IS_R &= ~0x01;   // Set PJ0 as edge-sensitive
    GPIO_PORTJ_AHB_IBE_R &= ~0x01;  // Set PJ0 as single edge-triggered
    GPIO_PORTJ_AHB_IEV_R &= ~0x01;  // Set PJ0 to trigger on falling edge
    GPIO_PORTJ_AHB_IM_R |= 0x01;    // Enable interrupt mask for PJ0
    GPIO_PORTJ_AHB_ICR_R = 0x01;   // Clear pj0 interrupt



    // Configure interrupt for SW1 (PJ1)
    GPIO_PORTJ_AHB_IS_R &= ~0x02;   // Set PJ0 as edge-sensitive
    GPIO_PORTJ_AHB_IBE_R &= ~0x02;  // Set PJ0 as single edge-triggered
    GPIO_PORTJ_AHB_IEV_R &= ~0x02;  // Set PJ0 to trigger on falling edge
    GPIO_PORTJ_AHB_IM_R |= 0x02;    // Enable interrupt mask for PJ0
    GPIO_PORTJ_AHB_ICR_R = 0x02;   // Clear pj1 interrupt


    // Set the priority of the interrupt (adjust as needed)
    //IntPrioritySet(INT_GPIOJ, 0x20); // Assuming INT_GPIOJ is defined as 51

    // Enable the interrupt globally
    //IntEnable(INT_GPIOJ);


    // Set GPIOJ interrupt priority using NVIC_PRI12 register
    NVIC_PRI12_R &= ~(0x7 << 29); // Clear priority bits for IRQ #51 for port j, note that the register uses interrupt numer
    // and not nested vector number
    NVIC_PRI12_R |= (0x2 << 29); // Set priority to 0x2


    // Enable the GPIOJ iterrupt (assuming INT_GPIOJ is defined as 51)
    NVIC_EN1_R |= (1U << 19);  // Enable interrupt 51 (adjust for NVIC_ENx_R)

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

        /* Delay for a 100ms */
        for (ulLoop = 0; ulLoop < delay_value; ulLoop++){}

        /* Turn off the LED */
        GPIO_PORTN_DATA_R &= ~(3 << 0);

        /* Delay for a 100ms */
        for (ulLoop = 0; ulLoop < delay_value; ulLoop++){}
    }
}

void PortJ_IntHandler(void)
{

    volatile int readback;
    while (GPIO_PORTJ_AHB_MIS_R != 0)
    {
        if (GPIO_PORTJ_AHB_MIS_R & 0x01)
        {
            //toogle the the second led
            interruptSw0Blinking();
        }
        else if (GPIO_PORTJ_AHB_MIS_R & 0x02)
        {
            interruptSw1Blinking();
        }
        else
        {
            // clear all the interrupt
            GPIO_PORTJ_AHB_ICR_R = 0x02;
        }

    }



}

void interruptSw0Blinking(void)
{
    int i;
    for (i = 0; i < 3; i++)
    {
        // Your interrupt handling code here
        GPIO_PORTF_AHB_DATA_R |= (1UL << 4UL);

        for (ulLoop = 0; ulLoop < delay_value; ulLoop++){}

        GPIO_PORTF_AHB_DATA_R &= ~(1UL << 4UL);

        for (ulLoop = 0; ulLoop < delay_value; ulLoop++){}
    }

    GPIO_PORTJ_AHB_ICR_R = 0x01;   // Clear pj0 interrupt
}


void interruptSw1Blinking(void)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        // Your interrupt handling code here
        GPIO_PORTF_AHB_DATA_R |= (1UL << 0UL);

        for (ulLoop = 0; ulLoop < delay_value; ulLoop++){}

        GPIO_PORTF_AHB_DATA_R &= ~(1UL << 0UL);

        for (ulLoop = 0; ulLoop < delay_value; ulLoop++){}
    }

    GPIO_PORTJ_AHB_ICR_R = 0x02;   // Clear pj1 interrupt
}
