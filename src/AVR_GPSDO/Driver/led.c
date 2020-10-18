/*****************************************************
Title   :   LED Driver
Author  :   kingyo
File    :   led.c
DATE    :   2020/10/18
******************************************************/
#include "led.h"
#include <avr/io.h>


#define DEF_LED_PORT_ALM    (PORTC)
#define DEF_LED_BIT_ALM     (2)
#define DEF_LED_PORT_LOCK   (PORTC)
#define DEF_LED_BIT_LOCK    (3)

// LEDドライバ初期化
void LED_Init(void)
{
    /* Do nothing */
    // PORT設定はmcu.cで実施
}

// LED点灯状態設定
void LED_SetState(ENUM_LED_SEL sel, ENUM_LED_STATE st)
{
    switch (sel)
    {
        case ENUM_LED_SEL_ALM:
            if (st == ENUM_LED_STATE_ON)
            {
                DEF_LED_PORT_ALM |= 1 << DEF_LED_BIT_ALM;
            }
            else if (st == ENUM_LED_STATE_OFF)
            {
                DEF_LED_PORT_ALM &= ~(1 << DEF_LED_BIT_ALM);
            }
            break;

        case ENUM_LED_SEL_LOCK:
            if (st == ENUM_LED_STATE_ON)
            {
                DEF_LED_PORT_LOCK |= 1 << DEF_LED_BIT_LOCK;
            }
            else if (st == ENUM_LED_STATE_OFF)
            {
                DEF_LED_PORT_LOCK &= ~(1 << DEF_LED_BIT_LOCK);
            }
            break;

        case ENUM_LED_SEL_PPS:
            // PPS表示灯はGPSのPPS信号に直結のためMCUから制御しない
            break;

        default:
            break;
    }
}

// LED点灯状態取得
ENUM_LED_STATE LED_GetState(ENUM_LED_SEL sel)
{
    ENUM_LED_STATE tRet = ENUM_LED_STATE_NULL;

    switch (sel)
    {
        case ENUM_LED_SEL_ALM:
            tRet = (ENUM_LED_STATE)((DEF_LED_PORT_ALM >> DEF_LED_BIT_ALM) & 0x01);
            break;

        case ENUM_LED_SEL_LOCK:
            tRet = (ENUM_LED_STATE)((DEF_LED_PORT_LOCK >> DEF_LED_BIT_LOCK) & 0x01);
            break;

        case ENUM_LED_SEL_PPS:
            // PPS表示灯はGPSのPPS信号に直結のためMCUから制御しない
            break;

        default:
            break;
    }

    return tRet;
}
