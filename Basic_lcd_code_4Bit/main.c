/**
 * main.c
 */

/* note that  all interface uses only port B unlike 8 bit that we use both port a and port B*/
#include <stdio.h>
#include <stdint.h>
#include <tm4c129encpdt.h>

#define RS  1  // writing 1 to pin 0
#define RW  2  // writting 1 to pin 1
#define EN  4  // writing 1 to pin 2

void delayMs(int n);
void delayUs(int n);
void LCD_Command(unsigned char command);
void LCD_Data(unsigned char data);
void LCD_Init();
void LCD_Nibble_Write( unsigned char data, unsigned char control);



// writting the delay function in millisecond
void delayMs(int n){
    int i,j;
    for(i=0; i<n; i++)
        for(j=0; j<3180; j++)
        {}                         // do nothing for 1Ms
}

// writting the delay function for for n microseconds
void delayUs(int n){
    int i,j;
    for(i=0; i<n; i++)
        for(j=0; j<3; j++)
        {}                         // do nothing for n microsends
}

// writting the lcd command function
void LCD_Command(unsigned char command){

    /*GPIO_PORTA_AHB_DATA_R &= 0X00;   // SETTING THE Rs and R/W = 0 in GPIO PORTA

    GPIO_PORTB_AHB_DATA_R |= command; // enabling THE COMMAND IN GPIO PORTB

    GPIO_PORTA_AHB_DATA_R |= EN;       // SETTING GPIO PORTA TO ENABLE THE ENABLE REGISTER
    // WHICH IS THE PULSE
    delayUs(0);                         // DELAYING IN MICRO SECONDS

    GPIO_PORTA_AHB_DATA_R &= 0X00;       // CLEARING GPIO PORTA*/

    LCD_Nibble_Write(command & 0xF0,0);  // upper nibble first
    LCD_Nibble_Write(command<<4,0);        // LOWER NIBBLE



    if ( command < 4){                   // siNce the clock speed of the lcd is low
                                         // clear display screen and return cursor home take roughl 1.64ms to display
        delayUs(2);
    }
    else {
        delayUs(40);                     //ALL OTHER REQUIRE 40 MICRO SECONDS
    }

}

void LCD_Data(unsigned char data){

   /* GPIO_PORTA_AHB_DATA_R |= RS;       // MAKING THE Rs REGISTER TO 1 WHILE R/W =0

    GPIO_PORTB_AHB_DATA_R |= data;     // ENABLING THE COMMAND IN GPIO PORTB

    GPIO_PORTA_AHB_DATA_R |= (EN|RS);  // SENDING PULSE BY SETTING THE ENABLE REGISTER TO 1
    // AND SETTING THE  REGISTER SELECT TO 1
    delayUs(0);                        // fake a delay

    GPIO_PORTA_AHB_DATA_R &= 0X00;       // CLEARING GPIO PORTB

    delayUs(40);*/

    LCD_Nibble_Write(data & 0xF0,RS);  // upper nibble first

    LCD_Nibble_Write(data<<4,RS);  // lower nibble

    delayUs(40);

}

void LCD_Init(){
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1|SYSCTL_RCGCGPIO_R0; // ACTIVATING PORTA AND PORTB CLOCK REGISTER

    while(!( SYSCTL_RCGCGPIO_R & (SYSCTL_RCGCGPIO_R1|SYSCTL_RCGCGPIO_R0))); // WAITING FOR THE CLOCK TO SETTLLE DOWN

    GPIO_PORTB_AHB_DIR_R |= 0xFF;   // ENABLINGN ALL THE PINS OF PORTB

    GPIO_PORTB_AHB_DEN_R |= 0xFF;   // ENABLING THE PORTS FOR DIGITAL FUNCTION

    //INITILALIZATION OF THE SEQUENCE BY BLINKING 0 3 TIMES

    delayMs(20);
    LCD_Nibble_Write(0x30,0);
    delayMs(5);
    LCD_Nibble_Write(0x30,0);
    delayUs(100);
    LCD_Nibble_Write(0x30,0);
    delayUs(40);

    // initializing our cursors

    LCD_Nibble_Write(0x20,0);    // WE USE THE ASCII CODE 0x20 TO SET 4 BIT
    delayUs(40);
    LCD_Command(0x28);    // set 4 bit data 2 line 5x7 font
    LCD_Command(0x06);    // move cursor right
    LCD_Command(0x01);    // CLEAR SCREEN, MOVE CURSOR HOME
    LCD_Command(0x0F);    // TURN ON DISPLAY CURSOR BLINKING
}

void LCD_Nibble_Write( unsigned char data, unsigned char control){   // THIS FUNCTION BREAKS THE DATA TO 4 BITS

    data &=0xF0;          // clear lower nibble for controll in 4 bit lcd
    control &=0x0F;       // clear the upper nibble for data in 4 bit
    GPIO_PORTB_AHB_DATA_R |= (data|control);// where Rs and R/W =0
    GPIO_PORTB_AHB_DATA_R |= (data|control|EN);// sending pulse
    delayUs(0);
    GPIO_PORTB_AHB_DATA_R |=data;
    GPIO_PORTB_AHB_DATA_R |= 0x00;

}

int main(void)
{

    LCD_Init();

    while(1){
        LCD_Command(1);   // CLEAR DISPLAY OF THE SCREEN
        LCD_Command(0x80); // LCD CURSOR LOCATION AND FORCE IT HOME

        delayMs(500);
        LCD_Data('H');
        LCD_Data('E');
        LCD_Data('L');
        LCD_Data('L');
        LCD_Data('O');
        LCD_Data(' ');
        LCD_Data('S');
        LCD_Data('I');
        LCD_Data('.');
        delayMs(500);
    }
}
