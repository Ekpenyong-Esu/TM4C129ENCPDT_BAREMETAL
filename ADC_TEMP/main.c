#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "inc/tm4c129encpdt.h"
#include "USART.h"
#include <string.h>
#include "driverlib/sysctl.h"


void Timer0A_Init(void);
void Timer0A_Handler(void);
void tempUSARTPrint(void);
void ADC_Init(void);

int main(void)
{
  SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 25000000);
  // Enable ADC0 clock
  SYSCTL_RCGCADC_R |= SYSCTL_RCGCADC_R0;
  while (!(SYSCTL_PRADC_R & SYSCTL_PRADC_R0)) {}

  // Enable ADC GPIO clock
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;
  while (!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R4)) {}

  // Enable Timer 0 and Timer A clock
  SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0;
  while (!(SYSCTL_PRTIMER_R & SYSCTL_PRTIMER_R0)) {}

  //ADC0_CC_R = ((ADC0_CC_R & ~ADC_CC_CLKDIV_M) | (0 << ADC_CC_CLKDIV_S)) | ((ADC0_CC_R & ~ADC_CC_CS_M) | ADC_CC_CS_PIOSC);
  // GPIO config
  GPIO_PORTE_AHB_DEN_R &= ~(1 << 3); // Digital disable
  GPIO_PORTE_AHB_AMSEL_R |= (1 << 3); // Analog enable
  GPIO_PORTE_AHB_AFSEL_R |= (1 << 3); // Alternate function

  // Initialize the microcontroller, UART, ADC, and Timer
      UART0_Config(9600);
      ADC_Init();
      Timer0A_Init();

  while (1)
      {
          // Check if ADC conversion is complete
          if (ADC0_RIS_R & ADC_RIS_INR3)
          {
              // Clear the ADC interrupt flag
               ADC0_ISC_R = ADC_ISC_IN3;

              // Perform temperature calculation and UART output
              tempUSARTPrint();

          }

          // Your application code here
      }

}

void Timer0A_Init(void)
{
  // Configure Timer A of Timer 0
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN;         // Disable Timer A

  // For ADC to work with timer, Enable TIMER_ADCEV_TATOADCEN and TIMER_CTL_TAOTE

  TIMER0_CTL_R |= TIMER_CTL_TAOTE;    //  enable timer0A trigger to ADC
  TIMER0_ADCEV_R |= TIMER_ADCEV_TATOADCEN;//timer0A time-out event ADC trigger enabled
  TIMER0_CFG_R = TIMER_CFG_32_BIT_TIMER;   // Configure as 32-bit timer
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;  // Periodic timer mode
  TIMER0_TAILR_R = 5000000;                // Set the load value (e.g., for a 1 Hz timer)
  TIMER0_CC_R &= ~TIMER_CC_ALTCLK;    //  timer0 clocked from system clock
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;        // Enable Timer A time-out interrupt
  TIMER0_CTL_R |= TIMER_CTL_TAEN;          // Enable Timer A


  NVIC_PRI4_R = (NVIC_PRI4_R & ~NVIC_PRI4_INT19_M) | (1 << NVIC_PRI4_INT19_S); // Timer0A interrupt priority
  // Enable the Timer0A interrupt
  NVIC_EN0_R |= 1 << (INT_TIMER0A - 16);
}

void ADC_Init(void)
{

  // Initialize ADC0
  ADC0_ACTSS_R &= ~ADC_ACTSS_ASEN3; // Disable sample sequencer 3 during configuration
  ADC0_EMUX_R = ADC_EMUX_EM3_TIMER; // Set EM3 field for Timer trigger
  ADC0_SSMUX3_R = 0; // Get input from channel 0
  ADC0_SSCTL3_R |= ADC_SSCTL3_TS0 | ADC_SSCTL3_IE0 | ADC_SSCTL3_END0; // Take chip temperature, set flag at 1st sample
  ADC0_SSPRI_R = 0x0123;              //  sequencer 3 is lowest priority (default)
  ADC0_IM_R &= ~ADC_IM_MASK3;          //  enable SS3 interrupts
  ADC0_ACTSS_R |= ADC_ACTSS_ASEN3; // Enable sample sequencer 3

}

void Timer0A_Handler(void)
{
    // Clear the Timer A time-out interrupt
    TIMER0_ICR_R = TIMER_ICR_TATOCINT;

    // Clear the ADC interrupt flag
     ADC0_ISC_R = ADC_ISC_IN3;

    // Start ADC conversion
    ADC0_PSSI_R = ADC_PSSI_SS3;

}


void tempUSARTPrint(void)
{
    char buffer[50];
    int temp = ADC0_SSFIFO3_R;
    // Read the temperature value from the ADC FIFO register
    double temp_C = 147 - (247 * (double) temp) / 4096;
    // Print the integer `123` to the buffer.
    printf("The temperature value is: %.2f\370C\r\n", temp_C);
    // Use printf to send data via UART0
    printf("Hello, UART!\n");

    sprintf(buffer, "The temperature value is: %.2f\370C\r\n", temp_C);

    UART0_SendString(buffer);
    UART0_SendChar('\r');
    UART0_SendChar('\n');
}
