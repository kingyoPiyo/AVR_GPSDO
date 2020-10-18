/*****************************************************
Title   :   TIMER Driver
Author  :   kingyo
File    :   timer.h
DATE    :   2020/07/05
******************************************************/
#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

/*********************************
 * Typedef
 *********************************/
typedef void (* FUNC_POINTER)(void);

/*********************************
 * Prptotype
 *********************************/
void TIMER_Init(void);
uint32_t TIMER_GetElapsedTime(uint32_t baseTime);
uint32_t TIMER_GetNowTimeCnt(void);
void TIMER_SetOneshotFunc(FUNC_POINTER f, uint32_t time);
void TIMER_SetLcdDuty(uint8_t duty);

#endif /* __TIMER_H__ */
