#include <stdint.h>
#include "tm4c129encpdt.h"

#define DS1307_ADDR 0x68

void I2C0_init(void);
void I2C0_byteWrite(uint8_t slaveAddr, uint8_t regAddr, uint8_t data);

int main(void) {
    I2C0_init();

    // Set the time to 16:58:55
    I2C0_byteWrite(DS1307_ADDR, 0, 0x55); // Seconds
    I2C0_byteWrite(DS1307_ADDR, 1, 0x58); // Minutes
    I2C0_byteWrite(DS1307_ADDR, 2, 0x16); // Hours

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
}

void I2C0_byteWrite(uint8_t slaveAddr, uint8_t regAddr, uint8_t data) {
    // Wait until the I2C master is not busy
    while (I2C0_MCS_R & I2C_MCS_BUSY);

    // Set the slave address and write operation
    I2C0_MSA_R = (slaveAddr << 1) | 0; // Write operation

    // Write the register address
    I2C0_MDR_R = regAddr;

    // Start the I2C communication
    I2C0_MCS_R = (I2C_MCS_START | I2C_MCS_RUN);

    // Wait until the operation is complete
    while (I2C0_MCS_R & I2C_MCS_BUSY);

    // Write the data to the data register
    I2C0_MDR_R = data;

    // Start the I2C communication
    I2C0_MCS_R = (I2C_MCS_START | I2C_MCS_STOP | I2C_MCS_RUN);

    // Wait until the operation is complete
    while (I2C0_MCS_R & I2C_MCS_BUSY);
}
