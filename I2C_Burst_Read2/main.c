#include <stdint.h>
#include "tm4c129encpdt.h"

#define DS1307_ADDR 0x68

void I2C0_init(void);
void I2C0_burstRead(uint8_t slaveAddr, uint8_t startRegAddr, uint8_t* data, uint8_t numBytes);

int main(void) {
    uint8_t timeDateReadBack[7];

    I2C0_init();

    for (;;) {
        /* use burst read to read time and date */
        I2C0_burstRead(DS1307_ADDR, 0, timeDateReadBack, 7);
        /* display date and time here */
    }
}


void I2C0_init(void) {
    SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R0; // Enable clock to I2C0

    while(!(SYSCTL_PRI2C_R & SYSCTL_PRI2C_R0)){};  // check if the peripheral register is ready for GPIO(PRGPIO)

    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1; // Enable clock to GPIO Port B

    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1)){};  // check if the peripheral register is ready for GPIO(PRGPIO)

    // Configure GPIO pins for I2C0 (PB2: SCL and PB3: SDA)
    GPIO_PORTB_AHB_AFSEL_R |= (1 << 2) | (1 << 3);
    GPIO_PORTB_AHB_PCTL_R &= ~0xFFFF00FF;
    GPIO_PORTB_AHB_PCTL_R |= 0x00002200;
    GPIO_PORTB_AHB_DEN_R |= (1 << 2) | (1 << 3);
    GPIO_PORTB_AHB_ODR_R |= (1 << 3);

    // Set the clock speed (assuming a 16 MHz system clock) 16000000/20 * 100000
    I2C0_MTPR_R = 7; // for 100 kHz

    // Enable I2C0
    I2C0_MCR_R = I2C_MCR_MFE;
}

char I2C_wait_till_done(void) {
    while (I2C0_MCS_R & I2C_MCS_BUSY); // Wait until I2C master is not busy
    return I2C0_MCS_R & (I2C_MCS_ERROR | I2C_MCS_ADRACK);
}


void I2C0_burstRead(uint8_t slaveAddr, uint8_t startRegAddr, uint8_t* data, uint8_t numBytes) {

    char error;
    int i;
    // Send slave address and starting address for write
    I2C0_MSA_R = (slaveAddr << 1) | 0; // Write operation
    I2C0_MDR_R = startRegAddr;
    I2C0_MCS_R = (I2C_MCS_START | I2C_MCS_RUN);
    error = I2C_wait_till_done();
    if (error)
        return;

    // Switch to read mode with a repeated start
    I2C0_MSA_R = (slaveAddr << 1) | 1; // Read operation
    I2C0_MCS_R = (I2C_MCS_START | I2C_MCS_RUN);
    error = I2C_wait_till_done();
    if (error)
        return;

    for (i = 0; i < numBytes; i++) {
        if (i == numBytes - 1) {
            // Last byte, send NACK and stop
            I2C0_MCS_R = (I2C_MCS_RUN | I2C_MCS_STOP);
        } else {
            // ACK and continue
            I2C0_MCS_R = (I2C_MCS_RUN);
        }

        error = I2C_wait_till_done();
        if (error)
            return;

        data[i] = I2C0_MDR_R;
    }
}
