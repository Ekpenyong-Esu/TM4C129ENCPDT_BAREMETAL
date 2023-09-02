/**
 * main.c
 */
#include <drive/Key.h>

void keypad_init(){

    // SETTING FIRST THE ROW WHICH IS GPIO PORTE AND IS THE OUTPUP
    // ENABLING GPIO PORTE AND GPIO PORTC CLOCK
    SYSCTL_RCGCGPIO_R |= (SYSCTL_RCGCGPIO_R4|SYSCTL_RCGCGPIO_R2);


    //ALLOWING THE CLOCK TO SETTLE DOWN
    while(!( SYSCTL_RCGCGPIO_R & (SYSCTL_RCGCGPIO_R4|SYSCTL_RCGCGPIO_R2)));


    // SETTING THE GPIO PORTE PIN3 TO PIN0 AS OUTPUT AND ENABLING THEM AS GPIO OUTPUT
    GPIO_PORTE_AHB_DIR_R |= 0x0F;

    //AFTER SETTING IT AS OUTPUT WE NOW SET IT FOR DIGITAL ENABLE
    GPIO_PORTE_AHB_DEN_R |= 0x0F;

    // WE NOW SET THE PIN3 TO PIN0 AS OPEN DRAIN TO MAKE IT HIGH
    GPIO_PORTE_AHB_ODR_R |=  0x0F;



    // SETTING THE COLOUMN WHIC IS GPIPO PORTC PIN4 TO PIN7
    // WE FIRST SET THE DIR REGISTER TO 0 TO MAKE PC4 TO PC7 AS IN INPUT
    GPIO_PORTC_AHB_DIR_R &= ~0xF0;

    // WE NOW ENABLE THE DIGITAL FUNCTION OF PC4 TO PC7
    GPIO_PORTC_AHB_DEN_R |= 0xF0;

    // WE NOW ENABLE THE PULL UP RESISTOR REGISTER OF PORTE PC4 TO PC0 TO MAKE THEM 1s
    GPIO_PORTC_AHB_PUR_R |= 0xF0;
}


// tHis function is a non-blocking function TO READ KEYPAD
//if a key is pressed it returns a  KEY LABEL IN ASCII CODING
// if a key is not pressed it return a 0
unsigned char keypad_getkey(void){

    const unsigned char keymap[4][4] = {

                                        {'1', '2', '3', 'A'},
                                        {'4', '5', '6', 'B'},
                                        {'7', '8', '9', 'C'},
                                        {'*', '0', '#', 'D'},
    };

    uint8_t k_row, k_col;

    // WE FIRST TO CHECK IF ANY KEY PRESSED
    // WE ENABLE ALL THE ROWS
    GPIO_PORTE_AHB_DATA_R &= 0x00;

    // WE READ ALL COLOUMNS
    k_col = GPIO_PORTC_AHB_DATA_R & 0xF0;

    if (k_col == 0xF0)                                      // if no key is pressed
        return 0;                                         // return 0

    // if any key is pressed, it gets here to find out which key
    // although it is writen as an infinite loop, it will break out from one of the breaks

    while (1){

        k_row = 0;

        GPIO_PORTE_AHB_DATA_R |= 0x0E;                   //ENABLE THE ROW 0
        delayUs(2);

        k_col |= GPIO_PORTC_AHB_DATA_R & 0xF0;             // READ ALL COLOUMNS IN PORT C

        if (k_col != 0xF0)
            break;


        k_row = 1;
        GPIO_PORTE_AHB_DATA_R |= 0x0D;                   // ENABLE ROW 2
        delayMs(2);


        k_col |= GPIO_PORTC_AHB_DATA_R & 0xF0;

        if (k_col != 0xF0)
            break;

        k_row = 2;
        GPIO_PORTE_AHB_DATA_R |=0x0B;                    // ENABLE ROW 3
        delayMs(2);

        k_col |= GPIO_PORTC_AHB_DATA_R & 0xF0;

        if (k_col != 0xF0)
            break;

        k_row = 3;
        GPIO_PORTE_AHB_DATA_R |= 0x07;                   // ENABLING ROW 4
        delayMs(2);

        k_col |= GPIO_PORTC_AHB_DATA_R & 0xF0;

        if (k_col != 0xF0)
            break;

        return 0;                                       // if no key is pressed

    }


    // AFTER BREAKING, THE FLOW EXECUTION COMES HERE WHEN ONE OF THE KEY IS PRESSED

    if (k_col == 0xE0)
        return keymap[k_row][0];                      // WHEN KEY IN COLOUMN 0 IS PRESSED

    if (k_col == 0xD0)
        return keymap[k_row][1];                      // WHEN KEY IN COLOUMN 1 IS PRESSED

    if (k_col == 0xB0)
        return keymap[k_row][2];                       // WHEN KEY IN COLOUMN 2 IS PRESSED

    if (k_col == 0x70)
        return keymap[k_row][3];                       // WHEN KEY IN COLOUMN 3 IS PRESSED

    return 0;                                         // return 0 when no key is pressed


}


int main(void)
{
    unsigned char key;
    int key_count = 0;

    keypad_init();
    LCD_Init();

    while(1){

        LCD_Command(0x80); // LCD CURSOR LOCATION AND FORCE IT HOME

        // GET KEY FUNCTION IS CALLED HERE AND READ THE KEYPAD
        key = keypad_getkey();

        // IF KEYPAD IS  PRESSED
        if( key != 0){

            key_count++;

            if(key_count >= 16){

                LCD_Data(key);
                delayMs(500);
                LCD_Command(0xC0);
                key_count = 0;
            }

           if (key == '*'){
               LCD_Command(0x01);
             //  LCD_Command(0x08)
               key_count = 0;

           }

            // DISPLAY THE KEY LABEL
            LCD_Data(key);
            delayMs(100);
        }

        else{

            LCD_Data(' ');

            delayMs(20);
        }

    }
}
