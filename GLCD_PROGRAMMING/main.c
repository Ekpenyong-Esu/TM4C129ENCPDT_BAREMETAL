#include <stdint.h>
#include "tm4c129encpdt.h"

#define RESET 0x20 /* PC5 reset pin */
#define DC    0x40 /* PC6 register select pin */

#define GLCD_WIDTH  84
#define GLCD_HEIGHT 48

#define GPIO_PIN_3 (1 << 3)
#define GPIO_PIN_2 (1 << 2)
#define GPIO_PIN_1 (1 << 1)

void GLCD_setCursor(uint8_t x, uint8_t y);
void GLCD_clear(void);
void GLCD_init(void);
void GLCD_data_write(uint8_t data);
void GLCD_command_write(uint8_t data);
void SSI2_init(void);
void SSI2_write(uint8_t data);
void delayMs(uint32_t n);
void delayUs(uint32_t n);

int main(void)
{
    char font_table[][6] = {
        {0x7e, 0x11, 0x11, 0x11, 0x7e, 0},  /* A */
        {0x7f, 0x49, 0x49, 0x49, 0x36, 0},  /* B */
        {0x3e, 0x41, 0x41, 0x41, 0x22, 0}}; /* C */

    int i;

    GLCD_init();    /* initialize the GLCD controller */
    GLCD_clear();   /* clear display and home the cursor */

    /* display letter A */
    for (i = 0; i < 6; i++)
        GLCD_data_write(font_table[0][i]);

    /* display letter B */
    for (i = 0; i < 6; i++)
        GLCD_data_write(font_table[1][i]);

    /* display letter C */
    for (i = 0; i < 6; i++)
        GLCD_data_write(font_table[2][i]);

    while(1) { }
}

void GLCD_setCursor(uint8_t x, uint8_t y)
{
    GLCD_command_write(0x80 | x);  /* column and setting the 8bit to 1 to signal command*/
    GLCD_command_write(0x40 | y);  /* bank (8 rows per bank) */
}

void GLCD_clear(void)
{
    int32_t index;
    // The loop iterates over each byte of the GLCD, and the condition
    //The division by 8 is used because each byte represents 8 vertical pixels.
    for (index = 0 ; index < (GLCD_WIDTH * GLCD_HEIGHT / 8) ; index++)
        GLCD_data_write(0x00);  // clear the screen by writting a byte to it 0x00

    GLCD_setCursor(0, 0);       // set the cursor to the first line and at the beginning
}

void SSI2_init(void)
{
    SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R2;   /* enable clock to SSI2 */
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R2; /* enable clock to GPIOC  */
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3; /* enable clock to GPIOD */

    GPIO_PORTD_AHB_AFSEL_R |= GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1; /* PORTB 7, 5, 4 for SSI2 */
    GPIO_PORTD_AHB_PCTL_R &= ~0xF0FF0000; /* PORTB 7, 5, 4 for SSI2 */
    GPIO_PORTD_AHB_PCTL_R |= GPIO_PORTD_AHB_PCTL_R = (GPIO_PORTD_AHB_PCTL_R & ~0x0000FFF0) | 0x0000FFF0;
    GPIO_PORTD_AHB_DEN_R |= GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1; /* PORTB 7, 5, 4 as digital pins */

    GPIO_PORTC_AHB_DATA_R |= 0x60;        /* set PORTC 5, 6, 7 idle high */
    GPIO_PORTC_AHB_DIR_R |= 0x60;         /* set PORTC 5, 6, 7 as output for CS */
    GPIO_PORTC_AHB_AMSEL_R &= ~0x60;      /* disable analog */
    GPIO_PORTC_AHB_DEN_R |= 0x60;         /* set PORTC 5, 6, 7 as digital pins */

    SSI2_CR1_R = 0;      /* disable SSI2 and make it master */
    SSI2_CC_R = 0;       /* use system clock */
    SSI2_CPSR_R = 16;    /* clock prescaler divide by 16 gets 1 MHz clock */
    SSI2_CR0_R = 0x0007; /* clock rate div by 1, phase/polarity 0 0, mode freescale, data size 8 */
    SSI2_CR1_R |= SSI_CR1_SSE;      /* enable SSI2  */
}

void GLCD_init(void)
{
    SSI2_init();

    GPIO_PORTC_AHB_DATA_R &= ~RESET;
    GPIO_PORTC_AHB_DATA_R |= RESET;

    GLCD_command_write(0x21);
    GLCD_command_write(0xB0);
    GLCD_command_write(0x04);
    GLCD_command_write(0x14);
    GLCD_command_write(0x20);
    GLCD_command_write(0x0C);
}

void GLCD_data_write(uint8_t data)
{
    GPIO_PORTC_AHB_DATA_R |= DC;
    SSI2_write(data);
}

void GLCD_command_write(uint8_t data)
{
    GPIO_PORTC_AHB_DATA_R &= ~DC;
    SSI2_write(data);
}

void SSI2_write(uint8_t data)
{
    SSI2_DR_R = data;
    while (SSI2_SR_R & SSI_SR_BSY); /* wait until transmit complete */
    data = SSI2_DR_R;
}

void delayMs(uint32_t n)
{
    uint32_t i, j;
    for(i = 0; i < n; i++)
        for(j = 0; j < 3180; j++)
            {}
}

void delayUs(uint32_t n)
{
    uint32_t i, j;
    for(i = 0 ; i < n; i++)
        for(j = 0; j < 3; j++)
            {}
}
