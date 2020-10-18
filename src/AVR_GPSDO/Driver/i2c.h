/*****************************************************
Title   :   I2C Driver
Author  :   kingyo
File    :   i2c.h
DATE    :   2020/07/05
******************************************************/
#ifndef __I2C_H__
#define __I2C_H__

#include <stdint.h>

/*********************************
 * Prptotype
 *********************************/
void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_SendData(uint8_t data);
uint8_t I2C_ReadData(uint8_t ack);

#endif /* __I2C_H__ */
