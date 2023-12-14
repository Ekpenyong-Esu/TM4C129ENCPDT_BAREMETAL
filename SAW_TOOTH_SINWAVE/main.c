#include <math.h>
#include <stdint.h>
#include "tm4c129encpdt.h"

#define WAVEFORM_LENGTH 1024
int sinewave[WAVEFORM_LENGTH];

void init_sinewave();
void delayMs(uint32_t n);

int main(void)
{
    int i;
    // Initialize the sine wave data table.
    init_sinewave();

    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;   // Enable clock to GPIO Port B

    // Configure specific pins in Port B as outputs for DAC
    GPIO_PORTB_AHB_DIR_R |= 0xFF;             // Set PB0-7 as output
    GPIO_PORTB_AHB_DEN_R |= 0xFF;             // Enable digital pins

    while (1)
    {
        for (i = 0; i < WAVEFORM_LENGTH; i++)
        {
            GPIO_PORTB_AHB_DATA_R = sinewave[i];  // Send out the value from the sine wave

            if (i == WAVEFORM_LENGTH)
            {
                i = 0;
            }
            delayMs(1);

        }
    }
}

// Delay n milliseconds (assuming a 120 MHz CPU clock)
void delayMs(uint32_t n)
{
    int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < 3180; j++)
        {} // Do nothing for 1 ms
}

void init_sinewave()
{
    float fRadians = ((2 * M_PI) / WAVEFORM_LENGTH);
    int i;

    for (i = 0; i < WAVEFORM_LENGTH; i++)
    {
        sinewave[i] = 500 * (sinf(fRadians * i) + 1);
    }
}
