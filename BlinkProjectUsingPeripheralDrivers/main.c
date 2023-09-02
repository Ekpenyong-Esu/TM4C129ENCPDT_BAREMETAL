#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"




int main(void)
{
    // Set the clock using PLL to 120MHZ
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
    SYSCTL_CFG_VCO_480), 120000000);


    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);  // Enable the Port N
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);  // Enable the port F

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0);  // Make pin 1 and 0 of N output
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);    // Make pin 4 and 0 of F output

    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1 |GPIO_PIN_0, 0x00); // Make the pins LOW
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4 |GPIO_PIN_1, 0x00);    // Make the pins LOW

    while(1)
    {
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0x3);        // Write to the pins
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, (1<<4 | 1 << 0)); // Write to the pins
    SysCtlDelay(2000000);

    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, 0);
    SysCtlDelay(2000000);
    }

}
