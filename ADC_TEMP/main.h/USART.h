/*
 * USART.h
 *
 *  Created on: Sep 24, 2023
 *      Author: mahon
 */

#ifndef USART_H_
#define USART_H_

void UART0_Config(uint32_t baudRate) ;
void UART0_SendInt(int data);
void UART0_SendChar(char data);
char UART0_ReceiveChar(void);
void UART0_SendString(const char *str);

#endif /* USART_H_ */
