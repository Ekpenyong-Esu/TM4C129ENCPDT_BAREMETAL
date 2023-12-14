/*
 * USART.c
 *
 *  Created on: Sep 24, 2023
 *      Author: mahon
 */

#include <stdint.h>
#include <stdbool.h>
#include <tm4c129encpdt.h>
#include <stdio.h>
#include <stdlib.h>

// Function to configure UART0 for communication
void UART0_Config(uint32_t baudRate)
{
    // Enable the UART0 module

    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;  // 1 << 0

    // Enable clock to the corresponding GPIO port (Port A)
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;

    // check if the peripheral register is ready
    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R0)){};

    // Wait for the UART0 module to be ready
    while (!(SYSCTL_PRUART_R & SYSCTL_PRUART_R0)) {}

    // Set the UART pins (PA0: U0Rx, PA1: U0Tx)
    GPIO_PORTA_AHB_AFSEL_R |= (1 << 1 | 1 << 0);  // Set PA1 and PA0 alternate function

    // we first clear the register by using GPIO_PORTA_AHB_PCTL_R & 0xFFFFFF00 and the set porta pin 1 and pin 0
    //by using 0x00000011 we can still use (0x00000011) only or use (1 << 0) for pin PA0
    // and (1 << 4) for pin PA0
    GPIO_PORTA_AHB_PCTL_R = (GPIO_PORTA_AHB_AFSEL_R & ~0xFFFFFF00) | 0x00000011; // Enable UART on PA0 and PA1

    // Setting pin 0 and pin1 of port A for digital enable
    GPIO_PORTA_AHB_DEN_R  |= (1 << 1) | (1 << 0);


    // Disabling analog input in pin 1 and pin 0 of port A
    GPIO_PORTA_AHB_AMSEL_R &= ~(1 << 1 | 1 << 0);

    // Disable UART0 before configuring it
    UART0_CTL_R &= ~UART_CTL_UARTEN;


    // Set the baud rate
    uint32_t ui32UARTClk = 25000000; // Assuming 25 MHz system clock

    // calculating the Integer part of the baud rate
    uint32_t brdInteger = ui32UARTClk / (16 * baudRate);
    UART0_IBRD_R = brdInteger;


    // Calculating the fractional part of the baud rate
    // The simplest way is to multiply the fractional part by 64 and add 0.5
    uint32_t brdFraction = ((ui32UARTClk % (16 * baudRate)) * 64) + (baudRate / 2) / (baudRate);


    //uint32_t brdFraction1 = ((ui32UARTClk % (16 * baudRate)) * 64)

    UART0_FBRD_R = brdFraction;

    // Configure the UART data frame (8 data bits, 1 stop bit, no parity)
     UART0_LCRH_R = (UART_LCRH_WLEN_8 | UART_LCRH_FEN);

    // Enable UART0 and uart transfer and uart receive
    UART0_CTL_R |= (UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE);
}


// Function to send a single character via UART0
void UART0_SendChar(char data) {
    while ((UART0_FR_R & UART_FR_TXFF) != 0) {} // Wait for UART to become ready to transmit
    UART0_DR_R = data;                          // Send the data
}


// Function to receive a single character via UART0
char UART0_ReceiveChar(void) {
    while ((UART0_FR_R & UART_FR_RXFE) != 0) {} // Wait for UART to receive data
    return UART0_DR_R;                          // Read the received data
}

// Function to send a string via UART0
void UART0_SendString(const char *str) {
    while (*str != '\0')
    {
        UART0_SendChar(*str); // Send characters one by one
        str++;                 // Increment the string
    }
}


// Function to receive a string via UART0 (terminated by '\r' or '\n')
void UART0_ReceiveString(char *buffer, uint32_t bufferSize)
{
    char receivedChar;
    uint32_t count = 0;

    do {
        receivedChar = UART0_ReceiveChar();
        buffer[count] = receivedChar;
        count++;
    } while (receivedChar != '\r' && receivedChar != '\n' && count < bufferSize - 1);

    buffer[count - 1] = '\0'; // Null-terminate the received string
}


void UART0_SendInt(uint32_t data) {
    char buffer[20]; // Create a buffer to hold the string representation of the integer
    sprintf(buffer, "%d", data); // Convert the integer to a string

    UART0_SendString(buffer); // Send the string over UART
}

///// Print a character to UART.
//int fputchar(int ch, FILE *f) {
//    if ((ch == 10) || (ch == 13) || (ch == 27)) {
//        UART0_SendChar(13); // Carriage return
//        UART0_SendChar(10); // Line feed
//        return 1;
//    }
//    UART0_SendChar(ch);
//    return 1;
//}
//
//// Get a character from UART.
//int fgetc(FILE *f){
//  return UART0_ReceiveChar();
//}
//
//// Function called when file error occurs.
//int ferror(FILE *f){
//  /* Your implementation of ferror */
//  return EOF;
//}

int _write(int file, char *ptr, int len)
{
    if (file == 1) // Check if it's the stdout file descriptor
    {
        for (int i = 0; i < len; i++)
        {
            UART0_SendChar(*ptr++);
        }
        return len;
    }
    return -1; // Return an error if it's not stdout
}



