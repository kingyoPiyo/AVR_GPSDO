/*****************************************************
Title   :   Freq Control Service
Author  :   kingyo
File    :   FCNT_Service.c
DATE    :   2020/07/05
******************************************************/
#include "FCNT_Service.h"
#include "NMEA_Service.h"
#include "UI_Service.h"
#include "uart.h"
#include "i2c.h"
#include <util/atomic.h>
#include <stdio.h>

/*************************************************
 * Global Val
 ************************************************/
volatile static uint16_t gOvfCnt = 0;
volatile static uint8_t gCapISRFlag = 0;
volatile static uint32_t gCountVal = 0;
static uint32_t gCountVal_old = 0;
static uint16_t dacVal_old = 0;
static uint16_t tempVal = 0;
static uint32_t freq100_buf = 0;
static uint64_t freq1000_buf = 0;
static uint32_t FCNT_freq = 0;
static uint32_t FCNT_freq100 = 0;
static uint64_t FCNT_freq1000 = 0;
static uint16_t FCNT_avgCnt100 = 0;
static uint16_t FCNT_avgCnt1000 = 0;
static uint16_t FCNT_dacVal = 2048;

/*****************************
 * サービス初期化
 *****************************/
void FCNT_Init(void)
{
    gOvfCnt = 0;
    gCapISRFlag = 0;
    gCountVal = 0;
    gCountVal_old = 0;
    dacVal_old = 0;
    tempVal = 0;
    freq100_buf = 0;
    freq1000_buf = 0;

    FCNT_freq = 0;
    FCNT_freq100 = 0;
    FCNT_freq1000 = 0;
    FCNT_avgCnt100 = 0;
    FCNT_avgCnt1000 = 0;
    FCNT_dacVal = 2048;
}

/*****************************
 * キャプチャ割り込み
 *****************************/
ISR (TIMER1_CAPT_vect)
{
    // キャプチャレジスタ、オーバーフローカウンタの値を取得
    gCountVal = ICR1 + ((uint32_t)gOvfCnt << 16);
    gCapISRFlag = 1;

    // PPS音を鳴らす
    UI_PPSSound();
}

/*****************************
 * Timer1 Overflow
 *****************************/
ISR (TIMER1_OVF_vect)
{
    gOvfCnt++;
}

/*****************************
 * UARTログ送信
 *****************************/
static void uart_send(void)
{
    uint8_t i, j;
    char str[40];
    char uartTxBuf[100];

    j = 0;
    j = NMEA_GetUTCDate(j, uartTxBuf, 0);

    // NMEA情報
    j = NMEA_GetNMEA(j, uartTxBuf, 0);

    // 周波数値
    sprintf(str,"%08lu.%03lu %04d", (uint32_t)(FCNT_freq1000 / 1000), (uint32_t)(FCNT_freq1000 % 1000), FCNT_dacVal);
    for (i = 0; i < 17; i++)
    {
        uartTxBuf[j++] = str[i];
    }

    // 温度
    I2C_Start();
    I2C_SendData(0b10010000);   // Write
    I2C_SendData(0x00);         // Temp Register
    I2C_Start();
    I2C_SendData(0b10010001);   // Read
    tempVal = I2C_ReadData(1) << 8;
    tempVal |= I2C_ReadData(0);
    I2C_Stop();
    sprintf(str," %02d.%02d", (uint16_t)((float)tempVal * 0.78) / 100, (uint16_t)((float)tempVal * 0.78) % 100);
    for (i = 0; i < 6; i++)
    {
        uartTxBuf[j++] = str[i];
    }
    uartTxBuf[j++] = '\r';
    uartTxBuf[j++] = '\n';
    uartTxBuf[j++] = '\0';
    UART_TxString(uartTxBuf);
}

/*****************************
 * メインルーチン
 *****************************/
void FCNT_Main(void)
{
    // DACレジスタ設定
    if (FCNT_dacVal != dacVal_old)
    {
        I2C_Start();
        I2C_SendData(0b11000000);   // Write
        I2C_SendData((FCNT_dacVal & 0x0F00) >> 8);
        I2C_SendData((FCNT_dacVal & 0x00FF));
        I2C_Stop();
        dacVal_old = FCNT_dacVal;
    }

    // 周波数測定&制御
    if (gCapISRFlag) {
        gCapISRFlag = 0;

        ATOMIC_BLOCK(ATOMIC_FORCEON)
        {
            FCNT_freq = gCountVal - gCountVal_old;
            gCountVal_old = gCountVal;
        }

        // ハズレ値は加算しない
        if (FCNT_freq >= 9999995 && FCNT_freq <= 10000005)
        {
            // 100回単純平均
            if (FCNT_avgCnt100 >= 100) {
                FCNT_avgCnt100 = 1;
                FCNT_freq100 = freq100_buf;
                freq100_buf = FCNT_freq;
            } else {
                FCNT_avgCnt100++;
                freq100_buf += FCNT_freq;
            }

            // 1000回単純平均
            if (FCNT_avgCnt1000 >= 1000) {
                FCNT_avgCnt1000 = 1;
                FCNT_freq1000 = freq1000_buf;
                freq1000_buf = FCNT_freq;

                // DAC制御 ±1LSB
                if (FCNT_freq1000 > 10000000000) {
                    FCNT_dacVal+=1;
                } 
                else if (FCNT_freq1000 < 10000000000)
                {
                    FCNT_dacVal-=1;
                }

                // UARTログ送信
                uart_send();

            } else {
                FCNT_avgCnt1000++;
                freq1000_buf += FCNT_freq;
            }
        }
    }
}

/*****************************
 * 周波数測定値を返す
 *****************************/
uint64_t FCNT_GetFreq(ENUM_FCNT_FREQ_GATET gateTime, uint16_t *aveCnt)
{
    uint64_t ret;

    switch (gateTime)
    {
        case ENUM_FCNT_FREQ_GATET_1:
            ret = FCNT_freq;
            break;

        case ENUM_FCNT_FREQ_GATET_100:
            ret = FCNT_freq100;
            *aveCnt = FCNT_avgCnt100;
            break;

        case ENUM_FCNT_FREQ_GATET_1000:
            ret = FCNT_freq1000;
            *aveCnt = FCNT_avgCnt1000;
            break;
        
        default:
            ret = 0;
            break;
    }

    return ret;
}

/*****************************
 * DACレジスタ値を返す
 *****************************/
uint16_t FCNT_GetDacVal(void)
{
    return FCNT_dacVal;
}

/*****************************
 * DACレジスタ値を設定する
 *****************************/
void FCNT_SetDacVal(uint16_t dac)
{
    FCNT_dacVal = dac;
}
