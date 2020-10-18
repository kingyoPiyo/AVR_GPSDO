/*****************************************************
Title   :   Freq Control Service
Author  :   kingyo
File    :   FCNT_Service.h
DATE    :   2020/07/05
******************************************************/
#ifndef __FCNT_SERVICE_H__
#define __FCNT_SERVICE_H__

#include <stdint.h>

/*********************************
 * ENUM
 *********************************/
typedef enum {
    ENUM_FCNT_FREQ_GATET_1,     // Gate time = 1sec
    ENUM_FCNT_FREQ_GATET_100,   // Gate time = 100sec
    ENUM_FCNT_FREQ_GATET_1000   // Gate time = 1000sec
} ENUM_FCNT_FREQ_GATET;

/*********************************
 * Prptotype
 *********************************/
void FCNT_Init(void);
void FCNT_Main(void);
uint64_t FCNT_GetFreq(ENUM_FCNT_FREQ_GATET gateTime, uint16_t *aveCnt);
uint16_t FCNT_GetDacVal(void);
void FCNT_SetDacVal(uint16_t dac);

#endif //__FCNT_SERVICE_H__