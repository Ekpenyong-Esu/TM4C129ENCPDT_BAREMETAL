#include <stdbool.h>
#include <stdint.h>
#include "tm4c129encpdt.h"

#define LED1 GPIO_PORTN_DATA_R  // PN1
#define LED2 GPIO_PORTN_DATA_R  // PN0
#define LED3 GPIO_PORTF_AHB_DATA_R  // PF4
#define LED4 GPIO_PORTF_AHB_DATA_R  // PF0

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
    NVIC_ST_RELOAD_R = 16000000-1;

    // Use system clock, Enable systics and enable interrupt
    NVIC_ST_CTRL_R = NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_INTEN | NVIC_ST_CTRL_ENABLE; // Enable Systic, no interrupt and use the system clock


    /* Loop forever */
    while (1)
    {


    }
}

void SYSTICK_Handler(void)
{
    // Display the bits on the LEDs
    LED1 ^= 0x1UL << 1UL; // PN1
    LED2 ^= 0x1UL << 0UL; // PN0
    LED3 ^= 0x1UL << 4UL; // PF4
    LED4 ^= 0x1UL << 0UL; // PF0
}
