/*
 * USART.h
 *
 *  Created on: 7 Oct 2021
 *      Author: EKPENYONG ESU EKPENYONG
 */

#ifndef USART_H_
#define USART_H_



#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "inc/tm4c129encpdt.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"

//*****************************************************************************
//
// Definition of variables
//
//*****************************************************************************

uint32_t ui32SysClock;

//*****************************************************************************
//
// Interfaces of the functions
//
//*****************************************************************************

extern void USART_init();
extern void timer_init();
extern void TimerIntHandler(void);
extern char USART_getChar();
extern void USART_putChar(char c);
extern void USART_putString(char *string);
extern void USART_getString(char *string);






#endif /* USART_H_ */
