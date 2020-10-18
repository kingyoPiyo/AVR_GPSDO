/*****************************************************
Title   :   Sound Driver
Author  :   kingyo
File    :   sound.c
DATE    :   2020/10/11
******************************************************/
#include "sound.h"
#include <avr/io.h>


void SOUND_Init(void)
{
    /* Timer0 Setting */
    TCCR0A  = 0b00000010;   // OC0A pin toggle, CTC mode
    TCCR0B  = 0b00000100;   // clk/256
    OCR0A   = 128;
}

void SOUND_Stop(void)
{
    TCCR0A &= ~(1 << 6);
}

void SOUND_Start(uint8_t freq)
{
    OCR0A = freq;
    TCCR0A |= 1 << 6;
}
