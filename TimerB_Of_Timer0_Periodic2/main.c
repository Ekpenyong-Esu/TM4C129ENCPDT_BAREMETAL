/* Defines boolean and integer data types */
#include <stdbool.h>
#include <stdint.h>
#include "tm4c129encpdt.h"


#define SystemCoreClock 16000000

void TimeroB_Mill_Init(void)
{
    // Enable Timer0 Clock
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0;

    // Wait until the Timer0 clock is ready
    while ((SYSCTL_PRTIMER_R & SYSCTL_PRTIMER_R0) == 0) {}

    // Disable TimerB before configuration
    TIMER0_CTL_R &= ~TIMER_CTL_TBEN;

    // Set the timer to 16-bit mode
    TIMER0_CFG_R = TIMER_CFG_16_BIT ;

    // Set the TimerB to be in periodic mode
    TIMER0_TBMR_R = TIMER_TBMR_TBMR_PERIOD;
}

void TimeroB_Mill(uint32_t milliseconds)
{
    int i;
    // Calculate the number of clock cycles required for the delay
    uint32_t clockCycles = (uint32_t)((SystemCoreClock / 1000) * milliseconds);

    // Load the value into the TimerB
   TIMER0_TBILR_R = clockCycles - 1; // Subtract 1 to account for the zero-based counting of TimerB

   // Clear the interrupt flag
   TIMER0_ICR_R = TIMER_ICR_TBTOCINT;

    // Enable TimerB
    TIMER0_CTL_R |= TIMER_CTL_TBEN;

    for (i = 0; i < milliseconds; i++)
    {
    // Wait until the TimerB times out
    while ((TIMER0_RIS_R & TIMER_RIS_TBTORIS) == 0)
    {}

    // Clear the interrupt flag
    TIMER0_ICR_R = TIMER_ICR_TBTOCINT;


    // Disable TimerA before returning
    //TIMER0_CTL_R &= ~TIMER_CTL_TAEN;



    }

}



int main(void)
{

    TimeroB_Mill_Init();

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

        /* Delay for a 500ms */
        TimeroB_Mill(500);

        /* Turn off the LED */
        GPIO_PORTN_DATA_R &= ~(3 << 0);
        GPIO_PORTF_AHB_DATA_R &= ~(1UL << 4UL | 1UL << 0UL);

        /* Delay for a 100ms */
        TimeroB_Mill(500);
    }
}

