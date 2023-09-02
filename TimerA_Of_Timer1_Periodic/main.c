/* Defines boolean and integer data types */
#include <stdbool.h>
#include <stdint.h>
#include "tm4c129encpdt.h"


#define SystemCoreClock 16000000

void Time1A_second_Init(void)
{
    // Enable Timer1 Clock
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1;

    // Wait until the Timer1 clock is ready
    while ((SYSCTL_PRTIMER_R & SYSCTL_PRTIMER_R1) == 0) {}

    // Disable Timer1A before configuration
    TIMER1_CTL_R &= ~TIMER_CTL_TAEN;

    // Set the timer to 16-bit mode
    TIMER1_CFG_R = TIMER_CFG_16_BIT ;

    // Set the Timer1A to be in period mode
    TIMER1_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
}

void Timer1A_second(uint32_t seconds)
{
    int i;
    // Calculate the number of clock cycles required for the delay
    uint32_t clockCycles = (uint32_t)(SystemCoreClock / 250);

   // Load the value into the Timer1A
    TIMER1_TAILR_R = clockCycles - 1; // Subtract 1 to account for the zero-based counting of TimerA

    TIMER1_TAPR_R = 250-1;

    // Clear the interrupt flag
    TIMER1_ICR_R = TIMER_ICR_TATOCINT;


    // Enable Timer1A
    TIMER1_CTL_R |= TIMER_CTL_TAEN;

    for (i = 0; i < seconds; i++)
    {
    // Wait until the TimerA times out
    while ((TIMER1_RIS_R & TIMER_RIS_TATORIS) == 0)
    {

    }

    // Clear the interrupt flag
    TIMER1_ICR_R = TIMER_ICR_TATOCINT;


    // Disable TimerA before returning
    //TIMER0_CTL_R &= ~TIMER_CTL_TAEN;



    }

}



int main(void)
{

    Time1A_second_Init();

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
        Timer1A_second(1);

        /* Turn off the LED */
        GPIO_PORTN_DATA_R &= ~(3 << 0);
        GPIO_PORTF_AHB_DATA_R &= ~(1UL << 4UL | 1UL << 0UL);

        /* Delay for a 100ms */
        Timer1A_second(1);
    }
}

