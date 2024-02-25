#include <stdint.h>
#include "tm4c129encpdt.h"  // Include the header file for TM4C129ENC
#include <math.h>

#define WAVEFORM_LENGTH 1024 // Maximum value of the DAC
int sinewave[WAVEFORM_LENGTH];

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
    GPIO_PORTA_AHB_PCTL_R = (GPIO_PORTA_AHB_PCTL_R & ~0x000F0F00) | 0x000F0F00;  // Configure PA2,  PA4  as SSI
    GPIO_PORTA_AHB_AMSEL_R = 0;       // Disable analog functionality on PA2, PA4


    // Disable SSI0 during configuration
    SSI0_CR1_R = 0x0000;

    // Set the clock source for SSI0 (use system clock)
    SSI0_CC_R = 0x0;

    uint32_t systemClock = 16000000;  // 16 MHz
    uint32_t spiClock = 1000000;     // 1 MHz
    uint32_t prescaler = (systemClock / (spiClock * 2) - 1);

    // Configure SSI0 control register (CR0)
    SSI0_CR0_R = (SSI0_CR0_R & ~(SSI_CR0_SCR_M | SSI_CR0_SPO | SSI_CR0_SPH)) |
            SSI_CR0_FRF_MOTO |
            SSI_CR0_DSS_8 |  // 8 bit data size
            (prescaler << SSI_CR0_SCR_S);
            //SSI_CR0_SPH |     // Data captured on the second edge (rising edge)
            //SSI_CR0_SPO;     // Clock idles high
    SSI0_CR1_R &= ~SSI_CR1_MS; // Set SSI as master




    // Enable SSI0
    SSI0_CR1_R |= SSI_CR1_SSE;


//       SSI0_CR0_R = (SSI0_CR0_R & ~SSI_CR0_SCR_M) | (prescaler << SSI_CR0_SCR_S); // 16 as clock divisor (adjust as needed)
//       //SSI0_CR0_R = (SSI0_CR0_R & ~SSI_CR0_SPH) | SSI_CR0_SPH; // Data captured on second clock edge
//       //SSI0_CR0_R = (SSI0_CR0_R & ~SSI_CR0_SPO);  // clock polarity is 0 Idle clock is low
//       SSI0_CR0_R = (SSI0_CR0_R & ~SSI_CR0_FRF_M) | SSI_CR0_FRF_MOTO; // Motorola frame format
//       SSI0_CR0_R = (SSI0_CR0_R & ~SSI_CR0_DSS_M) | SSI_CR0_DSS_8; // 8-bit data
//       SSI0_CR1_R |= SSI_CR1_MS; // Set SSI as master
//       SSI0_CR1_R |= SSI_CR1_SSE; // Enable SSI

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

void LTC1661Write(int chan, uint16_t data)
{
    assertSSLow();     /* assert SS low */

    data = (data & 0x03FF) << 2;   /* bit 1-0 unused */

    if (chan == 0)                /* add control code with channel number */
        data |= 0x9000;         // When you or it the result will be 16 bit with with data matching the lower 10 bit of 0x9000
    else
        data |= 0xA000;

    while (!(SSI1_SR_R & SSI_SR_TNF)); /* wait until FIFO not full */

    SSI1_DR_R = data >> 8;       /* transmit high byte which has 0x90 */
    while (!(SSI1_SR_R & SSI_SR_TNF)){} /* wait until FIFO not full */

    SSI1_DR_R = data & 0xFF;          /* transmit low byte */
    while (SSI1_SR_R & SSI_SR_BSY){} /* wait until transmit complete */

    deassertSSHigh();    /* keep SS idle high */
}




int main(void)
{
    float fRadians;

    initSSI();

    /* Construct a data table for a sine wave */
    fRadians = ((2 * M_PI) / WAVEFORM_LENGTH);
    int i;
    for (i = 0; i < WAVEFORM_LENGTH; i++)
    {
        sinewave[i] = (uint16_t)(511 * (sinf(fRadians * i) + 1));
    }

    while (1)
    {

        int i;
        for (i = 0; i < WAVEFORM_LENGTH; i++)
        {
            /* Write a sine wave to channel B */
            LTC1661Write(1, sinewave[i]);
        }
    }
}


