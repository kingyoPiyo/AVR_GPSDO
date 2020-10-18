/*****************************************************
Title   :   UART Driver
Author  :   kingyo
File    :   uart.c
DATE    :   2020/07/04
******************************************************/
#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define NULL ((void*)0)

// コールバック関数
static FUNC_POINTER pRxCallBackFunc = NULL;

// 送信バッファ
static uint8_t txBuf[257];
// 送信サイズ
static uint16_t txSize;
// 送信済みサイズ
static uint16_t txdSize;

// 初期化
void UART_Init(void)
{
    // UART設定
    UCSR0A  = 0b00000000;
    UCSR0B  = 0b11011000;   // 受信・送信割り込み許可、送受信許可
    UCSR0C  = 0b00000110;   // Data Size = 8bit
    UBRR0   = 162;          // 9600bps@25MHz
}

// コールバック関数設定
void UART_SetRxCallBackFunc(FUNC_POINTER f)
{
    pRxCallBackFunc = f;
}

// 1Byte送信関数
void UART_TxByte(char data)
{
    // 送信中なら処理しない
    if (txdSize < txdSize) return;

    UDR0 = data;
}

// 文字列送信関数
void UART_TxString(const char *data)
{
    // 送信中なら処理しない
    if (txdSize < txdSize) return;

    // 送信データコピー
    txSize = 0;
    while ( (txBuf[txSize] = *data++) )
    {
        txSize++;
    }

    // 1Byte目送信
    UDR0 = txBuf[0];
    txdSize = 1;
}

// 受信完了割り込み関数
ISR (USART_RX_vect)
{
    if (pRxCallBackFunc != NULL)
    {
        pRxCallBackFunc();
    }
}

// 送信完了割り込み
ISR (USART_TX_vect)
{
    if (txdSize < txSize)
    {
        UDR0 = txBuf[txdSize];
        txdSize++;
    }
}
