#include <USART.h>

// This function initializes the USART driver
void USART_init(){

    //Definition of the clock
    //ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_OSC), 16000000);

    // Enable the GPIO Peripheral used by the UART.
    // which is GPIO PORTA
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //Wait for GPIOA to be ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)){}

    // Enable UART0 so tthat the clock can be configured
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //Wait for UART0 to be ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)){}

    // Configure GPIO Pins for the UART mode on PORTA pin 0 and 1.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Initialize the UART for console I/O with a baud rate of 115200 and with no parity and a clock of 16MHz
    //UARTStdioConfig(0, 9600, ui32SysClock);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    UARTStdioConfig(0, 115200, 16000000);

    //Enable UART0
    UARTEnable(UART0_BASE);
}

//This function is used to receive a character
char USART_getChar(){
    char c; //Character to be read
    while((UART0_FR_R&(UART_FR_RXFE)) != 0); //When the value is 1, there is no data to received

    //When the value is 0, there is data to received
    c = UART0_DR_R;
    return c;
}

//This function is used to transmit a character
void USART_putChar(char c){
    while((UART0_FR_R&(UART_FR_TXFF)) != 0); //When the value is 1,can't transmit

    //When the value is 0, you can transmit the character
    UART0_DR_R = c;
}

//This function is used to send a string
void USART_putString(char *string){
    while (*string!='\0'){
        USART_putChar(*(string++)); //Transmits the string separated in characters
    }
}

//This function is used to receive a string
void USART_getString(char *string){
    char *buff = string;
    char character = USART_getChar(); //Takes every character of the string
    while(character != '\r'){ //If the value is not enter, gets in
        *string = character;
        USART_putChar(character);
        string++; //Takes the next value
        character = USART_getChar(); //Takes the next character of the string
    }
    *string = '\0'; //It has transmitted all the string
    string = buff;
}
