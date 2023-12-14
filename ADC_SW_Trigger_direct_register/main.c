#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c129encpdt.h"
#include "driverlib/sysctl.h"
int main(void)
{

  SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 25000000);
  volatile int temperature;

  // Enable ADC0 clock
  SYSCTL_RCGCADC_R |= SYSCTL_RCGCADC_R0;
  while (!(SYSCTL_PRADC_R & SYSCTL_PRADC_R0)) {}

  // Enable ADC GPIO clock
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;
  while (!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R4)) {}

  //ADC0_CC_R = ((ADC0_CC_R & ~ADC_CC_CLKDIV_M) | (0 << ADC_CC_CLKDIV_S)) | ((ADC0_CC_R & ~ADC_CC_CS_M) | ADC_CC_CS_PIOSC);


  // GPIO config
  GPIO_PORTE_AHB_DEN_R &= ~(1 << 3); // Digital disable
  GPIO_PORTE_AHB_AMSEL_R |= (1 << 3); // Analog enable
  GPIO_PORTE_AHB_AFSEL_R |= (1 << 3); // Alternate function

  // Initialize ADC0
  ADC0_ACTSS_R &= ~ADC_ACTSS_ASEN3; // Disable sample sequencer 3 during configuration
  ADC0_EMUX_R |= ADC_EMUX_EM3_PROCESSOR; // Clear and set the EM3 field for softwaretrigger
  ADC0_SSMUX3_R = 0; // Get input from channel 0
  ADC0_SSCTL3_R |= ADC_SSCTL3_TS0 | ADC_SSCTL3_IE0 | ADC_SSCTL3_END0; // Take chip temperature, set flag at 1st sample

  // Enable sample sequencer 3
  ADC0_ACTSS_R |= ADC_ACTSS_ASEN3;



  while (1)
  {
      // Clear the ADC interrupt flag
      ADC0_ISC_R = ADC_ISC_IN3;

      // Start conversion
      ADC0_PSSI_R = ADC_PSSI_SS3;

      // Wait for conversion complete
      while ((ADC0_RIS_R & ADC_RIS_INR3) == 0) {}

      // Read the temperature value from the ADC FIFO register
      temperature = 147 - (247 * ADC0_SSFIFO3_R) / 4096;


  }
}
