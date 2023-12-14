/* I2C to I2C */

/* This program communicate with the DS1307 Real-time Clock via I2C. */
/* The seconds, minutes, and hours are written one byte at a time. */
/* Set the time to 16:58:55 */

/* DS1307 parameters:
   fmax = 100 kHz */

#include "tm4c129encpdt.h" // Include the header file for TM4C129ENCPDT
#include <stdbool.h>
#include <stdint.h>

#define SLAVE_ADDR 0x68

void I2C0_init(void);
char I2C0_byteWrite(int slaveAddr, char memAddr, char data);

int main(void) {
    char error;
    char timeDateToSet[7] = {0x55, 0x58, 0x16, 0x01, 0x19, 0x10, 0x09};

    I2C0_init();

    // Write hour, minute, second with single byte writes
    error = I2C0_byteWrite(SLAVE_ADDR, 0, timeDateToSet[0]); // Second
    if (error)
    {
        while (1);
    }

    error = I2C0_byteWrite(SLAVE_ADDR, 1, timeDateToSet[1]); // Minute
    if (error)
    {
        while (1);
    }

    error = I2C0_byteWrite(SLAVE_ADDR, 2, timeDateToSet[2]); // Hour
    if (error)
    {
        while (1);
    }

    while (1) {
        // Your application code here
    }
}

void I2C0_init(void) {
    SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R0; // Enable clock to I2C1

    while(!(SYSCTL_PRI2C_R & SYSCTL_PRI2C_R0)){};  // check if the peripheral register is ready for GPIO(PRGPIO)

    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1; // Enable clock to GPIO Port B
    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1)){};  // check if the peripheral register is ready for GPIO(PRGPIO)

    // Configure GPIO pins for I2C1 (PB2: SCL and PB3: SDA)
    GPIO_PORTB_AHB_AFSEL_R |= (1 << 2) | (1 << 3);
    GPIO_PORTB_AHB_PCTL_R &= ~0x0000FF00;
    GPIO_PORTB_AHB_PCTL_R |= 0x00002200;
    GPIO_PORTB_AHB_DEN_R |= (1 << 2) | (1 << 3);
    GPIO_PORTB_AHB_ODR_R |= (1 << 3);

    I2C0_MCR_R = I2C_MCR_MFE; // Master mode and enable

    // Configure I2C1 clock speed (100 kHz)  (1600000/20*100000)-1
    I2C0_MTPR_R = 7; // Assuming a 16 MHz system clock

    // Enable I2C1
    I2C0_MCR_R = I2C_MCR_MFE;
}

// Wait until I2C master is not busy and return error code
static int I2C_wait_till_done(void)
{
    while (I2C0_MCS_R & I2C_MCS_BUSY); // Wait until I2C master is not busy
    return I2C0_MCS_R & (I2C_MCS_ERROR | I2C_MCS_ADRACK);
}

// Write one byte only
// Byte write: S-(saddr+w)-ACK-maddr-ACK-data-ACK-P
char I2C0_byteWrite(int slaveAddr, char memAddr, char data) {
    char error;
    // Send slave address and starting address
    I2C0_MSA_R = (slaveAddr << 1) | 0; // Write operation
    I2C0_MDR_R = memAddr;
    I2C0_MCS_R = (I2C_MCS_START | I2C_MCS_RUN);

    error = I2C_wait_till_done(); // Wait until write is complete
    if (error) return error;

    // Send data
    I2C0_MDR_R = data;
    I2C0_MCS_R = (I2C_MCS_RUN | I2C_MCS_STOP);

    error = I2C_wait_till_done(); // Wait until write is complete

    return error; // Return error code
}
