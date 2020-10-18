/*****************************************************
Title   :   UART Driver
Author  :   kingyo
File    :   uart.h
DATE    :   2020/07/04
******************************************************/
#ifndef __UART_H__
#define __UART_H__

/*********************************
 * Typedef
 *********************************/
typedef void (* FUNC_POINTER)(void);

/*********************************
 * Prptotype
 *********************************/
void UART_Init(void);
void UART_SetRxCallBackFunc(FUNC_POINTER f);
void UART_TxByte(char data);
void UART_TxString(const char *data);

#endif /* __UART_H__ */
