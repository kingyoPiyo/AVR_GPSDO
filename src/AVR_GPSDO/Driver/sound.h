/*****************************************************
Title   :   Sound Driver
Author  :   kingyo
File    :   sound.h
DATE    :   2020/10/11
******************************************************/
#ifndef __SOUND_H__
#define __SOUND_H__

#include <stdint.h>

/*********************************
 * Prptotype
 *********************************/
void SOUND_Init(void);
void SOUND_Stop(void);
void SOUND_Start(uint8_t freq);

#endif /* __SOUND_H__ */
