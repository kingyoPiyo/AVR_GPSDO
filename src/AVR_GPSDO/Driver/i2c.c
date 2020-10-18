/*****************************************************
Title   :   I2C Driver
Author  :   kingyo
File    :   i2c.c
DATE    :   2020/7/5
******************************************************/
#include "i2c.h"
#include <avr/io.h>

// I2C初期化
void I2C_Init(void)
{
    TWBR    = 32;           // ボーレート
    TWCR    = 0b00000100;   // TWI動作許可
    TWSR    = 0b00000000;   // 前置分周1
}

// スタートコンディション発行
void I2C_Start(void)
{
    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
    while( !( TWCR & (1<<TWINT) ) );
}

// ストップコンディション発行
void I2C_Stop(void)
{
    TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
}

// データ1Byte送信
void I2C_SendData(uint8_t data)
{
    TWDR = data;
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
}

// データ受信
uint8_t I2C_ReadData(uint8_t ack)
{
    if (ack) {
        TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
    } else {
        TWCR = (1<<TWINT) | (1<<TWEN);
    }
    while (!(TWCR & (1<<TWINT)));
    return TWDR;
}
