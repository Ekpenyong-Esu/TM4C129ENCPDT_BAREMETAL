/* Defines boolean and integer data types */
#include <stdbool.h>
#include <stdint.h>
#include "tm4c129encpdt.h"

#define SystemCoreClock 16000000

void Time0A_second_Init(void)
{
    // Enable Timer0 Clock
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0;

    // Wait until the Timer0 clock is ready
    while ((SYSCTL_PRTIMER_R & SYSCTL_PRTIMER_R0) == 0) {}

    // Disable Timer0A before configuration
    TIMER0_CTL_R &= ~TIMER_CTL_TAEN;

    // Set the timer to 32-bit mode
    TIMER0_CFG_R = TIMER_CFG_32_BIT_TIMER;

    // Set the Timer0A to be in edge-time and capture mode
    TIMER0_TAMR_R = TIMER_TAMR_TACDIR | TIMER_TAMR_TACMR | TIMER_TAMR_TAMR_CAP; // Capture mode, edge-time mode

    // Configure Timer0A to capture on rising edges
    TIMER0_CTL_R |= TIMER_CTL_TAEVENT_POS;
}

int Timer0A_Periodic_Capture(void)
{
    int startEdge, endEdge, edgeTime;

    // Wait until capture event occurs
    while ((TIMER0_RIS_R & TIMER_RIS_CAERIS) == 0) {}

    // Clear the interrupt flag of timer a capture register
    TIMER0_ICR_R = TIMER_ICR_CAECINT;

    // Read the captured edge time
    startEdge = TIMER0_TAR_R;    // Timer A Register  It is primarily used for reading the
                                  //current count value of Timer A

    //startEdge = TIMER0_TAV_R;   //Timer A Value Register  It is used to read the captured value
                                   //of Timer A when an input signal transition (edge) is detected.

    // Wait until capture event occurs
    while ((TIMER0_RIS_R & TIMER_RIS_CAERIS) == 0) {}

    // Clear the interrupt flag of timer a capture register
    TIMER0_ICR_R = TIMER_ICR_CAECINT;

    // Read the captured edge time
    startEdge = TIMER0_TAR_R;    // Timer A Register  It is primarily used for reading the
    //current count value of Timer A

    //startEdge = TIMER0_TAV_R;   //Timer A Value Register  It is used to read the captured value
    //of Timer A when an input signal transition (edge) is detected.

    // Calculate the edge time between rising edges
    edgeTime = endEdge - startEdge;

    return edgeTime;
}

int main(void)
{
    Time0A_second_Init();

    /* Enable the GPIO port B that is used for input signal for timerA of timer0 */
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

    while (!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1))
    {
    } // check if the peripheral register is ready for GPIO(PRGPIO)

    // Enable clock for GPIO port N
    SYSCTL_RCGCGPIO_R |= SYSCTL_PRGPIO_R12;

    while (!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R12))
    {
    };

    // Set portN pin 1 and 0 as output pins
    GPIO_PORTN_DIR_R |= (1UL << 1UL) | (1UL << 0UL);

    // Enable digital function
    GPIO_PORTN_DEN_R |= (1UL << 1UL) | (1UL << 0UL);

    GPIO_PORTN_AFSEL_R = 0x00000000;

    // Enable alternate function on PB6
    GPIO_PORTB_AHB_AFSEL_R |= (1UL << 6UL);

    // Configure PB6 for alternate function 7 (Timer0A)
    GPIO_PORTB_AHB_PCTL_R &= ~(0x0F000000);
    GPIO_PORTB_AHB_PCTL_R |= 0x07000000;

    /* Loop forever */
    while (1)
    {
        /* Turn on the LED */
        GPIO_PORTN_DATA_R |= 3UL << 0UL;
        Timer0A_Periodic_Capture();

        /* Turn off the LED */
        GPIO_PORTN_DATA_R &= ~(3 << 0);
        Timer0A_Periodic_Capture();
    }
}
