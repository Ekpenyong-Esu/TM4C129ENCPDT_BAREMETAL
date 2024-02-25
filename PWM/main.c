
#include <stdint.h>
#include "tm4c129encpdt.h"  // Include the TM4C129ENCPDT header file

void initPWM() {
    // Enable the PWM module and the GPIO port F
    SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R0;    // Enable PWM module 0
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;  // Enable GPIO Port F

    // Wait for the PWM and GPIO modules to be ready
    while ((SYSCTL_PRPWM_R & SYSCTL_PRPWM_R0) == 0) {}
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5) == 0) {}

    // Configure the GPIO pin for PWM
    GPIO_PORTF_AHB_AFSEL_R |= 0x01;   // Enable alternate function on PF0
    GPIO_PORTF_AHB_PCTL_R &= ~0x0000000F;
    GPIO_PORTF_AHB_PCTL_R |= 0x00000006;  // Configure PF0 for PWM (MUX = 6)
    GPIO_PORTF_AHB_DEN_R |= 0x01;    // Enable digital I/O on PF1

    // Configure PWM module 0
    PWM0_0_CTL_R = 0;           // Disable PWM while configuring
    PWM0_0_GENA_R = 0x0000008C;  // Set PWM mode and down-counting
    PWM0_0_LOAD_R = 16000 - 1;   // Set load value for 1 kHz (assuming 16 MHz clock)
    PWM0_0_CMPA_R = 8000 - 1;    // Set duty cycle for 50% (assuming 16 MHz clock)
    PWM0_0_CTL_R |= 0x00000001;  // Enable PWM

    // Enable the PWM output
    PWM0_ENABLE_R |= PWM_ENABLE_PWM0EN;
}

int main(void) {
    initPWM();  // Initialize PWM

    while (1) {
        // Your main code here
    }
}
