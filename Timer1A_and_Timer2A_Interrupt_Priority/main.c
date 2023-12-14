// THis code can only give delay of up to 4 milliseconds


/* Defines boolean and integer data types */
#include <stdbool.h>
#include <stdint.h>
#include "tm4c129encpdt.h"

void delay(uint32_t count);


#define SystemCoreClock 16000000


// Set up Timer1A to 1Hz
void Timer1A_Init(void)
{

    // Enable TIMER1 Clock
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1;

    // Wait until the TIMER1 clock is ready
    while ((SYSCTL_PRTIMER_R & SYSCTL_PRTIMER_R1) == 0) {}

    // Disable TimerA before configuration
    TIMER1_CTL_R &= ~TIMER_CTL_TAEN;

    // Set the timer to 16-bit mode
    TIMER1_CFG_R = TIMER_CFG_16_BIT ;

    // Set the TimerA to be in a periodic mode
    TIMER1_TAMR_R = TIMER_TAMR_TAMR_PERIOD;

    TIMER1_TAPR_R = 250;  // this divide the system clock which is 16000000 by 250 = 64000

   // Load the value into the TimerA
    TIMER1_TAILR_R = 64000;  // This divide the remaining 64000 by the giving TAILR value which is 64000

    // Clear the interrupt flag
    TIMER1_ICR_R = TIMER_ICR_TATOCINT;

    // Configure Timer1A to trigger an interrupt when the timer reaches zero
    TIMER1_IMR_R |= TIMER_IMR_TATOIM;

    // Enable TimerA
    TIMER1_CTL_R |= TIMER_CTL_TAEN;

    // Set the interrupt priority for UART0
    NVIC_PRI5_R &= ~(0x7 << 13);
    NVIC_PRI5_R |= (0x7 << 13);

   // Enable the interrupt
    NVIC_EN0_R |= (1U << 21U);
}



// Set up Timer1A to 10Hz
void Timer2A_Init(void)
{

    // Enable TIMER1 Clock
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R2;

    // Wait until the TIMER1 clock is ready
    while ((SYSCTL_PRTIMER_R & SYSCTL_PRTIMER_R2) == 0) {}

    // Disable TimerA before configuration
    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;

    // Set the timer to 16-bit mode
    TIMER2_CFG_R = TIMER_CFG_16_BIT ;

    // Set the TimerA to be in a periodic mode and down counter note that
    // when TIMER_TAMR_TACDIR = 0 it is down counter
    TIMER2_TAMR_R = TIMER_TAMR_TAMR_PERIOD;

    TIMER2_TAPR_R = 250;  // this divide the system clock which is 16000000 by 250 = 64000

   // Load the value into the TimerA
    TIMER2_TAILR_R = 6400;  // This divide the remaining 64000 by the giving TAILR value which is 6400 to give 10Hz

    // Clear the interrupt flag
    TIMER2_ICR_R = TIMER_ICR_TATOCINT;

    // Configure TIMER2A to trigger an interrupt when the timer reaches zero
    TIMER2_IMR_R |= TIMER_IMR_TATOIM;

    // Enable TimerA
    TIMER2_CTL_R |= TIMER_CTL_TAEN;

    // Set the interrupt priority for UART0
    NVIC_PRI5_R &= ~(0x7 << 29);
    NVIC_PRI5_R |= (5 << 29);

   // Enable the interrupt
    NVIC_EN0_R |= (1U << 23U);
}



void TIMER1A_Handler(void)
{
    if (TIMER1_MIS_R & TIMER_MIS_TATOMIS)
    {
        /* Turn on the LED */
        GPIO_PORTN_DATA_R |= 0x3UL << 0UL;
        delay(500);

       /* Turn off the LED */
        GPIO_PORTN_DATA_R &= ~(0x3 << 0);
        delay(500);
    }

    // Clear the Timer1A interrupt flag
    TIMER1_ICR_R = TIMER_ICR_TATOCINT;
}


void TIMER2A_Handler(void)
{
    if (TIMER2_MIS_R & TIMER_MIS_TATOMIS)
    {
        GPIO_PORTF_AHB_DATA_R ^= 1UL << 4UL;
        delay(20);
        GPIO_PORTF_AHB_DATA_R ^= 1UL << 0UL;
        delay(20);
    }

    // Clear the Timer1A interrupt flag
    TIMER2_ICR_R = TIMER_ICR_TATOCINT;
}



void delay(uint32_t count)
{
    uint32_t i;
    volatile uint32_t j;
    for (i = 0; i < count; i++)
    {
        for (j = 0; j < 1000; j++)
        {
        } // Delay loop
    }
}


int main(void)
{

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

    Timer1A_Init();
    Timer2A_Init();

    /* Loop forever */
    while (1)
    {

    }
}

