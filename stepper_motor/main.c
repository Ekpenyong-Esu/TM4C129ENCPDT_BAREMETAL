#include <stdint.h>
#include "tm4c1294ncpdt.h"

#define STEP_PIN_PIN 0
#define DIR_PIN_PIN  1

void delay_ms(uint32_t ms) {
    // Simple delay function
    volatile uint32_t i, j;
    for (i = 0; i < ms; ++i)
        for (j = 0; j < 20000; ++j);
}

void GPIO_Init(void) {
    // Enable GPIOB clock
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

    // Wait until the GPIOB module is ready
    while (!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1));

    // Configure GPIO_PORTB_PIN_0 and GPIO_PORTB_PIN_1 as output
    GPIO_PORTB_AHB_DIR_R |= (1 << STEP_PIN_PIN) | (1 << DIR_PIN_PIN);

    // Ensure push-pull mode
    GPIO_PORTB_AHB_ODR_R &= ~((1 << STEP_PIN_PIN) | (1 << DIR_PIN_PIN));

    // Ensure high-speed mode
    GPIO_PORTB_AHB_DR8R_R |= (1 << STEP_PIN_PIN) | (1 << DIR_PIN_PIN);

    // Ensure no pull-up/pull-down
    GPIO_PORTB_AHB_PUR_R &= ~((1 << STEP_PIN_PIN) | (1 << DIR_PIN_PIN));
}

void stepper_rotate(uint8_t direction, uint16_t steps, uint16_t delay) {
    // Set direction pin
    if (direction == 1) {
        GPIO_PORTB_AHB_DATA_R |= (1 << DIR_PIN_PIN);  // Set DIR_PIN
    } else {
        GPIO_PORTB_AHB_DATA_R &= ~(1 << DIR_PIN_PIN);  // Reset DIR_PIN
    }

    // Generate steps
    uint16_t i;
    for (i = 0; i < steps; ++i) {
        GPIO_PORTB_AHB_DATA_R |= (1 << STEP_PIN_PIN);  // Set STEP_PIN
        delay_ms(1);  // Adjust delay for desired speed
        GPIO_PORTB_AHB_DATA_R &= ~(1 << STEP_PIN_PIN);  // Reset STEP_PIN
        delay_ms(delay);  // Adjust delay for desired speed
    }
}

int main(void) {
    // Initialize GPIO
    GPIO_Init();

    // Main loop
    while (1) {
        // Rotate stepper motor clockwise
        stepper_rotate(1, 200, 10);  // 200 steps, 10ms delay

        // Rotate stepper motor counterclockwise
        stepper_rotate(0, 200, 10);  // 200 steps, 10ms delay
    }
}
