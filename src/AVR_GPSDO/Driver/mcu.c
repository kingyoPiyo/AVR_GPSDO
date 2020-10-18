/*****************************************************
Title   :   MCU misc
Author  :   kingyo
File    :   mcu.c
DATE    :   2020/06/17
******************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "mcu.h"
#include "uart.h"
#include "i2c.h"
#include "timer.h"
#include "sound.h"
#include "encoder.h"

// MCU初期化
void MCU_Init(void)
{
    // ポートI/O設定
    PORTB   = 0b00000000;
    DDRB    = 0b00111110;   // PB6,7 = OSC, PB5-1 = LCD, PB0 = PPS input
    PORTC   = 0b00000000;   //
    DDRC    = 0b00111111;   // PC7 = NONE, PC6 = Reset, PC5,4 = I2C, PC3,2 = LCD, PC1 = LCD_PWM. PC0 = LCD
    PORTD   = 0b00000000;
    DDRD    = 0b11000010;   // PD7 = LCD, PD6 = SP, PD5 = 10MHz Input, PD4 = ENC_PSW, PD3 = ENC_B, PD2 = ENC_A, PD1 = UART Tx, PD0 = UART Rx

    // タイマ1設定（周波数カウンタ）
    TCCR1A  = 0b00000000;   // 標準動作
    TCCR1B  = 0b01000111;   // 立ち上がりエッジでキャプチャ,カウンタクロック=T1ピンの立ち上がり
    TIMSK1  = 0b00100001;   // Timer/Counter1 Input Capture Interrupt Enable, Overflow Enable

    // 各種ドライバ初期化
    TIMER_Init();
    SOUND_Init();
    UART_Init();
    I2C_Init();
    ENCODER_Init();

    // ADT7410
    // A6~A2 = 0b10010
    // Addr = 0x48
    I2C_Start();
    I2C_SendData(0b10010000);   // Write
    I2C_SendData(0x03); // Config Register
    I2C_SendData(0x80); // Set 16bit Mode
    I2C_Stop();

    // DACレジスタ設定
    I2C_Start();
    I2C_SendData(0b11000000);   // Write
    I2C_SendData(0b10011000);
    I2C_Stop();

    // WDT設定
    cli();
    wdt_reset();
    WDTCSR |= (1<<WDCE)|(1<<WDE);
    WDTCSR = (1<<WDE)|(1<<WDP2)|(1<<WDP0);
}

// 全体割り込み許可
void MCU_EnableGlobalInterrupt(void)
{
    sei();
}

// WDTリセット
void MCU_ResetWDT(void)
{
    wdt_reset();
}
