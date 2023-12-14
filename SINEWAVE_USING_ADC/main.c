
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "tm4c129encpdt.h"  // Include the header file for TM4C129ENC
#include "driverlib/sysctl.h"

#define NUM_SAMPLES 256
#define AMPLITUDE   2047

void initSSI() {
    // Enable the SSI0 peripheral

    SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R0;

    while (!(SYSCTL_PRSSI_R & SYSCTL_PRSSI_R0));  // Enable SSI0 clock

    // Enable the SSI0 pins for communication
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;

    while (!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R0));

    // Configure PA3 as a regular GPIO output for SS
    GPIO_PORTA_AHB_DIR_R |= (1 << 3);  // Set PA3 as an output
    GPIO_PORTA_AHB_DEN_R |= (1 << 3);
    GPIO_PORTA_AHB_DATA_R |= (1 << 3); // Set PA3 high (deassert SS)


    GPIO_PORTA_AHB_AFSEL_R |= 0x5 << 2;   // Enable alternative functions on PA2, PA4
    GPIO_PORTA_AHB_DEN_R |=  0x5 << 2;     // Enable digital I/O on PA2,  PA4
    GPIO_PORTA_AHB_PCTL_R = (GPIO_PORTA_AHB_PCTL_R & ~0x000F0F00) | 0x00F0F00;  // Configure PA2,  PA4  as SSI
    GPIO_PORTA_AHB_AMSEL_R = 0;       // Disable analog functionality on PA2, PA4


    // Disable SSI0 during configuration
    SSI0_CR1_R = 0x0000;

    // Set the clock source for SSI0 (use system clock)
    SSI0_CC_R = 0x0;

    uint32_t systemClock = 16000000;  // 16 MHz
    uint32_t spiClock = 1000000;     // 1 MHz
    uint32_t prescaler = (systemClock / spiClock - 1);

    // Configure SSI0 control register (CR0)
    SSI0_CR0_R = (SSI0_CR0_R & ~(SSI_CR0_SCR_M | SSI_CR0_SPO | SSI_CR0_SPH)) |
            SSI_CR0_FRF_MOTO |
            SSI_CR0_DSS_8 |  // 16 bit data size
            (prescaler << SSI_CR0_SCR_S);
            //SSI_CR0_SPH |     // Data captured on the second edge (rising edge)
            //SSI_CR0_SPO;     // Clock idles high




    // Enable SSI0
    SSI0_CR1_R |= SSI_CR1_SSE;
}

/* 16 MHz SYSCLK */
void delayMs(int n) {
    int i;
    for (; n > 0; n--)
        for (i = 0; i < 3195; i++) ;
}

void assertSSLow() {
    GPIO_PORTA_AHB_DATA_R &= ~(1 << 3);  // Set PA3 low
}

// Function to deassert SS signal high
void deassertSSHigh() {
    GPIO_PORTA_AHB_DATA_R |= (1 << 3);   // Set PA3 high
}

void sendSSI(uint16_t data)
{
    assertSSLow();
    while ((SSI0_SR_R & SSI_SR_TNF) == 0) {}  // Wait until the transmit FIFO is not full

    SSI0_DR_R = data;  // Send data

    while ((SSI0_SR_R & SSI_SR_BSY) == 0) {}
    deassertSSHigh();
    //delayMs(1);

}


int main(void)
{
    //SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 25000000);
    // Initialize the system clock, GPIO, and SSI0
    // (Assume these are properly configured)

    initSSI();

    // Main loop for generating and transmitting the sine wave
    while (1)
    {

        int i;
        for (i = 0; i < NUM_SAMPLES; i++)
        {
            float angle = (2.0 * 3.14159265359 / NUM_SAMPLES) * i;
            uint16_t value = (uint16_t)(AMPLITUDE * sinf(angle));

            sendSSI(value);
            //delayMs(1);

        }
    }
}


