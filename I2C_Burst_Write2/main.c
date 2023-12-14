#include <stdint.h>
#include "tm4c129encpdt.h"

#define DS1307_ADDR 0x68

void I2C0_init(void);
void I2C0_burstWrite(uint8_t slaveAddr, uint8_t startRegAddr, uint8_t* data, uint8_t numBytes);

int main(void) {
    uint8_t timeDateToSet[7] = {0x55, 0x58, 0x16, 0x01, 0x19, 0x10, 0x09};

        // Perform a burst write to set time on DS1307
        I2C0_burstWrite(DS1307_ADDR, 0, timeDateToSet, 7);

    while (1) {
        // Your application code here
    }
}


void I2C0_init(void) {
    SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R0; // Enable clock to I2C0

    while(!(SYSCTL_PRI2C_R & SYSCTL_PRI2C_R0)){};  // check if the peripheral register is ready for GPIO(PRGPIO)

    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1; // Enable clock to GPIO Port B

    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1)){};  // check if the peripheral register is ready for GPIO(PRGPIO)

    // Configure GPIO pins for I2C1 (PB2: SCL and PB3: SDA)
    GPIO_PORTB_AHB_AFSEL_R |= (1 << 2) | (1 << 3);
    GPIO_PORTB_AHB_PCTL_R &= ~0xFFFF00FF;
    GPIO_PORTB_AHB_PCTL_R |= 0x00002200;
    GPIO_PORTB_AHB_DEN_R |= (1 << 2) | (1 << 3);
    GPIO_PORTB_AHB_ODR_R |= (1 << 3);


    // Initialize I2C0
    I2C0_MCR_R = I2C_MCR_MFE; // Enable I2C0 as a master

    // Set the clock speed (assuming a 16 MHz system clock)
    I2C0_MTPR_R = 7; // for 100 kHz

    // Enable I2C0
    I2C0_MCR_R = I2C_MCR_MFE;
}


void I2C0_burstWrite(uint8_t slaveAddr, uint8_t startRegAddr, uint8_t* data, uint8_t numBytes) {

    int i;
    // Wait until the I2C master is not busy
    while (I2C0_MCS_R & I2C_MCS_BUSY);

    // Set the slave address and write operation
    I2C0_MSA_R = (slaveAddr << 1) | 0; // Write operation

    // Write the start register address
    I2C0_MDR_R = startRegAddr;

    // Start the I2C communication
    I2C0_MCS_R = (I2C_MCS_START | I2C_MCS_RUN);

    // Wait until the operation is complete
    while (I2C0_MCS_R & I2C_MCS_BUSY);

    // Write the data bytes in a burst
    for (i = 0; i < numBytes; i++) {
        // Write each data byte
        I2C0_MDR_R = data[i];

        // Continue the I2C communication
        I2C0_MCS_R = (I2C_MCS_RUN);

        // Wait until the operation is complete
        while (I2C0_MCS_R & I2C_MCS_BUSY);
    }

    // Terminate the I2C communication with a stop condition
    I2C0_MCS_R = (I2C_MCS_STOP | I2C_MCS_RUN);

    // Wait until the stop condition is complete
    while (I2C0_MCS_R & I2C_MCS_BUSY);
}
