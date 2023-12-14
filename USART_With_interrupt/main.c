#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <tm4c129encpdt.h>

// Global variables

#define MAX_STRING_LENGTH  100

char receivedString[MAX_STRING_LENGTH];
uint32_t receivedStringLength = 0;


// Function to configure UART0 for communication
void UART0_Config(uint32_t baudRate)
{

    // Disable UART0 before configuring it
    UART0_CTL_R &= ~UART_CTL_UARTEN;

    // Set the baud rate
    uint32_t ui32UARTClk = 16000000; // Assuming 16 MHz system clock

    // calculating the Integer part of the baud rate
    uint32_t brdInteger = ui32UARTClk / (16 * baudRate);
    UART0_IBRD_R = brdInteger;

    // Calculating the fractional part of the baud rate
    // The simplest way is to multiply the fractional part by 64 and add 0.5
    uint32_t brdFraction = ((ui32UARTClk % (16 * baudRate)) * 64)
            + (baudRate / 2) / (baudRate);

    //uint32_t brdFraction1 = ((ui32UARTClk % (16 * baudRate)) * 64)

    UART0_FBRD_R = brdFraction;

    // Configure the UART data frame (8 data bits, 1 stop bit, no parity) and fifo enable FEN
    UART0_LCRH_R = (UART_LCRH_WLEN_8);

    // Enable UART0 receive interrupt
    UART0_IM_R |= UART_IM_RXIM; // enable RX interrupt

    // Enable UART0 and uart transfer and uart receive
    UART0_CTL_R |= (UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE);

    // Set the interrupt priority for UART0
    NVIC_PRI0_R &= ~(0x7 << 13);
    NVIC_PRI0_R |= 0x7 << 13;

    // Enable the interrupt
    NVIC_EN0_R |= (1U << 5U);

}

// Function to send a single character via UART0
void UART0_SendChar(char data)
{
    while ((UART0_FR_R & UART_FR_TXFF) != 0)
    {
    } // Wait for UART to become ready to transmit
    UART0_DR_R = data;                          // Send the data
}

// Function to send a string via UART0
void UART0_SendString(const char *str)
{
    while (*str != '\0')
    {
        UART0_SendChar(*str); // Send characters one by one
        str++;                 // Increment the string
    }
}

void UART0_IRQHandler(void)
{
    if (UART0_RIS_R & UART_RIS_RXRIS)
    {
        // UART0 received a character
        char receivedChar = UART0_DR_R;

        // Check if the received character is a newline ('\r' or '\n') indicating the end of the string
        if (receivedChar == '\r')
        {
            // Terminate the string with a null character
            receivedString[receivedStringLength++] = '\n';
            receivedStringLength = 0;

        }
        else
        {
            receivedString[receivedStringLength++] = receivedChar;
        }

        if (receivedStringLength >= MAX_STRING_LENGTH)
        {
            //UART0_IM_R &= ~UART_IM_RXIM; // Disable RX interrupt
            receivedStringLength = 0;    // Reset index if buffer is full
        }

        // Clear the interrupt
        UART0_ICR_R = UART_ICR_RXIC;
    }
}

void delayMs(int n)
{
    int i;
    for (; n > 0; n--)
        for (i = 0; i < 2000; i++)
            ;
}

int main(void)
{
    // Enable the UART0 module
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;

    // Wait for the UART0 module to be ready
    while (!(SYSCTL_PRUART_R & SYSCTL_PRUART_R0))
    {
    }

    // Enable clock to the corresponding GPIO port (Port A)
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;

    // check if the peripheral register is ready
    while (!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R0))
    {
    };

    // Set the UART pins (PA0: U0Rx, PA1: U0Tx)
    GPIO_PORTA_AHB_AFSEL_R |= (1 << 1 | 1 << 0); // Set PA1 and PA0 alternate function

    // we first clear the reguster by using GPIO_PORTA_AHB_PCTL_R & 0xFFFFFF00 and the set porta pin 1 and pin 0
    //by using 0x00000011 we can still use (0x00000011) only or use (1 << 0) for pin PA0
    // and (1 << 4) for pin PA0
    GPIO_PORTA_AHB_PCTL_R = (GPIO_PORTA_AHB_AFSEL_R & 0xFFFFFF00) | 0x00000011; // Enable UART on PA0 and PA1

    // Setting pin 0 and pin1 of port A for digital enable
    GPIO_PORTA_AHB_DEN_R |= (1 << 1) | (1 << 0);

    // Disabling analog input in pin 1 and pin 0 of port A
    GPIO_PORTA_AHB_AMSEL_R &= ~(1 << 1 | 1 << 0);

    // Configure UART0 with a baud rate of 9600
    UART0_Config(9600);
    UART0_SendString("Printing using USART with INTERRUPT: \r\n");

    while (1)
    {

        // Receive a string from the UART0 (terminated by '\r' or '\n')
        UART0_SendString("Entered String: ");
        delayMs(5000);
        UART0_SendString("\r\n"); // Newline character and Carriage return character

        if (receivedStringLength > 0)
        {

            UART0_SendString("String Received: ");

            UART0_SendString(receivedString);
            UART0_SendString("\r\n"); // Newline character and Carriage return character

            // Clear the buffer
            memset(receivedString, 0, sizeof(receivedString));
            receivedStringLength = 0;

        }

    }
}
