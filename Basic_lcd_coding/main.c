/**
 * main.c
 */
#include <stdio.h>
#include <stdint.h>
#include <tm4c129encpdt.h>

#define RS  0x20  // writing 1 to port 5 which can also be (1<<5)
#define RW  ox40  // writting 1 to port 6 which can be(1<<6)
#define EN  0x80  // writing 1 port 7 which can be (1<<7)

void delayMs(int n);
void delayUs(int n);
void LCD_Command(unsigned char command);
void LCD_Data(unsigned char data);
void LCD_Init();



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

    GPIO_PORTA_AHB_DATA_R &= 0X00;   // SETTING THE Rs and R/W = 0 in GPIO PORTA and when Rs is 0 we send command

    GPIO_PORTB_AHB_DATA_R |= command; // enabling THE COMMAND IN GPIO PORTB

    GPIO_PORTA_AHB_DATA_R |= EN;       // SETTING GPIO PORTA TO ENABLE THE ENABLE REGISTER
                                       // WHICH IS THE PULSE
    delayUs(0);                         // DELAYING IN MICRO SECONDS

    GPIO_PORTA_AHB_DATA_R &= 0X00;       // CLEARING GPIO PORTA

    if ( command < 4){                   // siNce the clock speed of the lcd is low
                                         // clear display screen and return cursor home take roughl 1.64ms to display
        delayUs(2);
    }
    else {
        delayUs(40);                     //ALL OTHER REQUIRE 40 MICRO SECONDS
    }

}

void LCD_Data(unsigned char data){

    GPIO_PORTA_AHB_DATA_R |= RS;       // MAKING THE Rs REGISTER TO 1 WHILE R/W =0 when Rs = 1, we send data

    GPIO_PORTB_AHB_DATA_R |= data;     // ENABLING data IN GPIO PORTB

    GPIO_PORTA_AHB_DATA_R |= (EN|RS);  // SENDING PULSE BY SETTING THE ENABLE REGISTER TO 1
                                       // AND SETTING THE  REGISTER SELECT TO 1
    delayUs(0);                        // fake a delay

    GPIO_PORTA_AHB_DATA_R &= 0X00;       // CLEARING GPIO PORTA

    delayUs(40);

}

void LCD_Init(){
    SYSCTL_RCGCGPIO_R |= (SYSCTL_RCGCGPIO_R1|SYSCTL_RCGCGPIO_R0); // ACTIVATING PORTA AND PORTB CLOCK REGISTER

    while(!( SYSCTL_RCGCGPIO_R & (SYSCTL_RCGCGPIO_R1|SYSCTL_RCGCGPIO_R0))); // WAITING FOR THE CLOCK TO SETTLLE DOWN

    GPIO_PORTA_AHB_DIR_R |= 0xE0;  // ACTIVATING PIN 7-5 OF PORT A FOR GPIO OUTPUT
                                   // THIS CAN ALSO BEGOTTEN WITH (1<<7)|(1<<6)|(1<<5)
    GPIO_PORTA_AHB_DEN_R |= 0xE0;   // ACTIVATING GPIO PORT A FOR DIGITAL FUNCTION FOR PIN 7-5
                                    //THIS CAN ALSO BEGOTTEN WITH (1<<7)|(1<<6)|(1<<5)
    GPIO_PORTB_AHB_DIR_R |= 0xFF;   // ENABLINGN ALL THE PORT OF GPIO PORT B FOR DATA AND COMMAND

    GPIO_PORTB_AHB_DEN_R |= 0xFF;   // ENABLING THE PORTS FOR DIGITAL FUNCTION

    //INITILALIZATION OF THE SEQUENCE BY BLINKING 0 3 TIMES

    delayMs(20);
    LCD_Command(0x30);
    delayMs(5);
    LCD_Command(0x30);
    delayUs(100);
    LCD_Command(0x30);

    // initializing our cursors

    LCD_Command(0x38);    // WE USE THE ASCII CODE 0x38 TO SET 8 BIT DATA, 2-LINE 5x7 FONT
    LCD_Command(0x06);    // MOVE CURSOR RIGHT
    LCD_Command(0x01);    // CLEAR SCREEN, MOVE CURSOR HOME
    LCD_Command(0x0F);    // TURN ON DISPLAY CURSOR BLINKING
}

int main(void)
{

    LCD_Init();

    while(1){
        LCD_Command(1);   // CLEAR DISPLAY OF THE SCREEN
        LCD_Command(0x80); // LCD CURSOR LOCATION AND FORCE IT HOME
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
