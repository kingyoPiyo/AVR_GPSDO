/*****************************************************
Title   :   LED Driver
Author  :   kingyo
File    :   led.h
DATE    :   2020/10/18
******************************************************/
#ifndef __LED_H__
#define __LED_H__

/*********************************
 * ENUM
 *********************************/
typedef enum {
    ENUM_LED_STATE_OFF = 0,
    ENUM_LED_STATE_ON,
    ENUM_LED_STATE_NULL
} ENUM_LED_STATE;

typedef enum {
    ENUM_LED_SEL_PPS = 0,
    ENUM_LED_SEL_LOCK,
    ENUM_LED_SEL_ALM
} ENUM_LED_SEL;

/*********************************
 * Prptotype
 *********************************/
void LED_Init(void);
void LED_SetState(ENUM_LED_SEL sel, ENUM_LED_STATE st);
ENUM_LED_STATE LED_GetState(ENUM_LED_SEL sel);

#endif /* __LED_H__ */
