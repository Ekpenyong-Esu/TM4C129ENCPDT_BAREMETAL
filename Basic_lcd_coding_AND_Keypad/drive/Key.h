/*
 * Key.h
 *
 *  Created on: 19 Nov 2021
 *      Author: MAHONRI
 */

#ifndef DRIVE_KEY_H_
#define DRIVE_KEY_H_

#include <stdio.h>
#include <stdint.h>
#include <tm4c129encpdt.h>

#define RS  0x20  // writing 1 to port 5 which can also be (1<<5)
#define RW  ox40  // writting 1 to port 6 which can be(1<<6)
#define EN  0x80  // writing 1 port 7 which can be (1<<7)

unsigned char key;


extern void delayMs(int n);
extern void delayUs(int n);
extern void LCD_Command(unsigned char command);
extern void LCD_Data(unsigned char data);
extern void LCD_Init();
extern unsigned char keypad_getkey(void);
extern void keypad_init();
extern void clear_keypadScreen();


#endif /* DRIVE_KEY_H_ */
