#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <driverlib/rom.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "inc/tm4c129encpdt.h"

#include "driverlib/rom_map.h"

#include "driverlib/adc.h"


int main(void)
{
    uint32_t ADCvalue[1];
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 25000000);
    // ADC config
      //ADCClockConfigSet(ADC0_BASE, ADC_CLOCK_SRC_PLL | ADC_CLOCK_RATE_FULL, 1);

    // Enable clocks
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);  // Enable clock to GPIO Port E (AIN0 is on PE3)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);   // Enable clock to ADC0

    // Initialize PE3 for AIN0 input
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);

    // Initialize ADC0
    ADCSequenceDisable(ADC0_BASE, 3);             // Disable sequencer 3 for configuration
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0); // Software trigger, priority 0
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END); // AIN0, set flag at 1st sample
    ADCSequenceEnable(ADC0_BASE, 3);              // Enable sequencer 3

    while(1)
    {
        ADCIntClear(ADC0_BASE, 3);                // Clear ADC interrupt flag
        ADCProcessorTrigger(ADC0_BASE, 3);        // Start a conversion sequence 3
        while(!ADCIntStatus(ADC0_BASE, 3, false)); // Wait for conversion complete
        ADCSequenceDataGet(ADC0_BASE, 3, ADCvalue); // Read conversion result
        // Read the temperature value from the ADC FIFO register
        int temperature = 147 - (247 * ADCvalue[0] / 4096);
    }
}
