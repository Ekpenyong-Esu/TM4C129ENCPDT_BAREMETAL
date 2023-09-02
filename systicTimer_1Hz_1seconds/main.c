#include <stdbool.h>
#include <stdint.h>
#include "tm4c129encpdt.h"

#define LEDN1 GPIO_PORTN_DATA_R  // PN1
#define LEDN0 GPIO_PORTN_DATA_R  // PN0
#define LEDF4 GPIO_PORTF_AHB_DATA_R  // PF4
#define LEDF0 GPIO_PORTF_AHB_DATA_R  // PF0


#define LED_BIT0 (1UL << 0UL)
#define LED_BIT1 (1UL << 1UL)
#define LED_BIT2 (1UL << 2UL)
#define LED_BIT3 (1UL << 3UL)
#define LED_BIT4 (1UL << 4UL)



int main(void)
{
    volatile uint32_t systick;

    /* Enable the GPIO port that is used for the onboard LED */
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;

    while (!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5));  // check if the peripheral register is ready for GPIO(PRGPIO)


    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12;

    while (!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R12));

    /* Do a dummy read to insert a few cycles after peripheral enable */
    // ulLoop = SYSCTL_RCGCGPIO_R;

    // Set portN pin 1 AND O as output pin
    GPIO_PORTN_DIR_R |= (1UL << 1UL | 1UL << 0UL);

    // Enabling digital function.
    GPIO_PORTN_DEN_R |= (1UL << 1UL | 1UL << 0UL);

    // Set portF pin 4 AND O as output pin
    GPIO_PORTF_AHB_DIR_R |= (1UL << 4UL | 1UL << 0UL);

    // Enabling digital function.
    GPIO_PORTF_AHB_DEN_R |= (1UL << 4UL | 1UL << 0UL);


    // Disable the alternate function of the pins
    GPIO_PORTN_AFSEL_R = 0x00;
    GPIO_PORTF_AHB_AFSEL_R = 0x00;


    // Configure systick
    NVIC_ST_RELOAD_R = 16000000-1; // reloading with the number of clock per seconds
    NVIC_ST_CTRL_R = NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_ENABLE; // Enable Systic, no interrupt and use the system clock


    /* Loop forever */
    while (1)
    {
        if (NVIC_ST_CTRL_R & (1UL << 16))
        {
            // Display the bits on the LEDs
            LEDN1 ^= LED_BIT1; // PN1 (Bit 0)
            LEDN0 ^= LED_BIT0; // PN0 (Bit 1)
            LEDF4 ^= LED_BIT4; // PF4 (Bit 2)
            LEDF0 ^= LED_BIT0; // PF0 (Bit 3)
        }

    }
}
