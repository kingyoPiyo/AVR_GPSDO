/*****************************************************
Title   :   NMEA Service
Author  :   kingyo
File    :   NMEA_Service.c
DATE    :   2020/07/05
******************************************************/
#include "NMEA_Service.h"
#include "uart.h"

volatile static char rxData[256];
volatile static char gNMEA[17];
volatile static char gUTC_Date[17];
volatile static uint8_t gUART_pass;

/*****************************
 * UART 受信割り込み
 *****************************/
static void UartRxCallBack(void)
{
    static uint8_t cnt = 0;
    uint8_t rxByte = UDR0;
    uint8_t i;

    // スルーさせる
    if (gUART_pass)
    {
        UDR0 = rxByte;
        return;
    }

    rxData[cnt] = rxByte;
    if (rxByte != '\n')
    {
        cnt++;
        return;
    }

    // 一行受信完了
    cnt = 0;

    // $GPGGA
    if (rxData[0] == '$' && rxData[1] == 'G' && rxData[2] == 'P' &&
        rxData[3] == 'G' && rxData[4] == 'G' && rxData[5] == 'A')
    {
        for (i = 0; i < 255; i++) { if(rxData[i] == ',') break; }
        // UTC時刻
        gNMEA[0] = rxData[i+1];
        gNMEA[1] = rxData[i+2];
        gNMEA[2] = ':';
        gNMEA[3] = rxData[i+3];
        gNMEA[4] = rxData[i+4];
        gNMEA[5] = ':';
        gNMEA[6] = rxData[i+5];
        gNMEA[7] = rxData[i+6];
        gNMEA[8] = ' ';

        for (i++; i < 255; i++) { if(rxData[i] == ',') break; }
        // 緯度
        // 未使用

        for (i++; i < 255; i++) { if(rxData[i] == ',') break; }
        // 北緯 or 南緯
        // 未使用

        for (i++; i < 255; i++) { if(rxData[i] == ',') break; }
        // 経度
        // 未使用

        for (i++; i < 255; i++) { if(rxData[i] == ',') break; }
        // 東経 or 西経
        // 未使用

        for (i++; i < 255; i++) { if(rxData[i] == ',') break; }
        // 位置特定品質
        gNMEA[9] = rxData[i+1];
        gNMEA[10] = ' ';

        for (i++; i < 255; i++) { if(rxData[i] == ',') break; }
        // 使用衛星数
        gNMEA[11] = rxData[i+1];
        gNMEA[12] = rxData[i+2];

        // 以下未使用
        gNMEA[13] = ' ';
        gNMEA[14] = ' ';
        gNMEA[15] = ' ';
        gNMEA[16] = '\0';
    }
    
    else // $GPRMC
    if (rxData[0] == '$' && rxData[1] == 'G' && rxData[2] == 'P' &&
        rxData[3] == 'R' && rxData[4] == 'M' && rxData[5] == 'C')
    {
        for (i = 0; i < 255; i++) { if(rxData[i] == ',') break; }
        // UTC時刻
        // 未使用

        for (i++; i < 255; i++) { if(rxData[i] == ',') break; }
        // ステータス
        // 未使用

        for (i++; i < 255; i++) { if(rxData[i] == ',') break; }
        // 緯度
        // 未使用

        for (i++; i < 255; i++) { if(rxData[i] == ',') break; }
        // 北緯 or 南緯
        // 未使用

        for (i++; i < 255; i++) { if(rxData[i] == ',') break; }
        // 経度
        // 未使用

        for (i++; i < 255; i++) { if(rxData[i] == ',') break; }
        // 東経 or 西経
        // 未使用

        for (i++; i < 255; i++) { if(rxData[i] == ',') break; }
        // 地表における移動の速度
        // 未使用

        for (i++; i < 255; i++) { if(rxData[i] == ',') break; }
        // 地表における移動の真方位
        // 未使用

        for (i++; i < 255; i++) { if(rxData[i] == ',') break; }
        // UTC日付
        gUTC_Date[0] = '2';
        gUTC_Date[1] = '0';
        gUTC_Date[2] = rxData[i+5];
        gUTC_Date[3] = rxData[i+6];
        gUTC_Date[4] = '/';
        gUTC_Date[5] = rxData[i+3];
        gUTC_Date[6] = rxData[i+4];
        gUTC_Date[7] = '/';
        gUTC_Date[8] = rxData[i+1];
        gUTC_Date[9] = rxData[i+2];
        gUTC_Date[10] = ' ';
        gUTC_Date[11] = ' ';
        gUTC_Date[12] = ' ';
        gUTC_Date[13] = ' ';
        gUTC_Date[14] = ' ';
        gUTC_Date[15] = ' ';
        gUTC_Date[16] = '\0';
    }
}

/*****************************
 * 初期化
 *****************************/
void NMEA_Init(void)
{
    UART_SetRxCallBackFunc(UartRxCallBack);
    gUART_pass = 0;
}

/*****************************
 * NMEAデータコピー
 *****************************/
uint8_t NMEA_GetNMEA(uint8_t stIndex, char *buf, uint8_t fillNullChar)
{
    uint8_t i, j = stIndex;

    for (i = 0; i < 14; i++)
    {
        *(buf + j++) = gNMEA[i];
    }
    
    if (fillNullChar)
    {
        for (i = 0; i < 2; i++)
        {
            *(buf + j++) = ' ';
        }
        *(buf + j++) = '\0';
    }

    return j;
}

/*****************************
 * UTC日付コピー
 *****************************/
uint8_t NMEA_GetUTCDate(uint8_t stIndex, char *buf, uint8_t fillNullChar)
{
    uint8_t i, j = stIndex;

    for (i = 0, j = 0; i < 11; i++)
    {
        *(buf + j++) = gUTC_Date[i];
    }
    
    if (fillNullChar)
    {
        for (i = 0; i < 5; i++)
        {
            *(buf + j++) = ' ';
        }
        *(buf + j++) = '\0';
    }

    return j;
}

/*****************************
 * アラーム情報を返す
 *****************************/
uint8_t NMEA_GetAramStatus(void)
{
    return (gNMEA[9] == '0');
}

/*****************************
 * UARTスルー設定
 *****************************/
void NMEA_SetUartThrough(uint8_t throughEn)
{
    gUART_pass = throughEn ? 1 : 0;
}
