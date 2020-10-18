/*****************************************************
Title   :   TIMER Driver
Author  :   kingyo
File    :   timer.c
DATE    :   2020/07/05
******************************************************/
#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#define NULL ((void*)0)

volatile static uint32_t gTimeCnt;
volatile static FUNC_POINTER gpTimeEventOneShotFunc = NULL;
static uint32_t gOneShotTime = 0;
static uint8_t lcd_pwmcnt;
static uint8_t lcd_pwmduty;

// タイマ2 比較一致A割り込み
ISR (TIMER2_COMPA_vect)
{
    // LCDバックライトDuty制御
    if (++lcd_pwmcnt > 9) lcd_pwmcnt = 0;
    if (lcd_pwmcnt < lcd_pwmduty)
    {
        PORTC |= (1 << 1);
    }
    else 
    {
        PORTC &= ~(1 << 1);
    }
    
    // タイマカウンタインクリメント
    gTimeCnt++;

    // ワンショット関数実行
    if (gpTimeEventOneShotFunc != NULL)
    {
        if (gTimeCnt == gOneShotTime)
        {
            gpTimeEventOneShotFunc();
            gpTimeEventOneShotFunc = NULL;
        }
    }
}

// カウンタ値取得
static uint32_t atomic_getTimeCnt(void)
{
    uint32_t tTime;

    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
        tTime = gTimeCnt;
    }

    return tTime;
}

// 初期化
void TIMER_Init(void)
{
    // タイマ2設定（1ms定周期）
    TCCR2A  = 0b00000010;   // 比較一致タイマ/カウンタクリア(CTC)動作
    TCCR2B  = 0b00000101;   // 128分周
    TIMSK2  = 0b00000010;   // 比較A割り込み許可
    OCR2A   = 195 - 1;      // 25MHz / 128 / 195 = 1001.6Hz

    gTimeCnt = 0;
}

// 経過時間を返す
uint32_t TIMER_GetElapsedTime(uint32_t baseTime)
{
    return (atomic_getTimeCnt() - baseTime);
}

// 現在カウント値を返す
uint32_t TIMER_GetNowTimeCnt(void)
{
    return atomic_getTimeCnt();
}

// ワンショット関数を登録
void TIMER_SetOneshotFunc(FUNC_POINTER f, uint32_t time)
{
    gpTimeEventOneShotFunc = f;
    gOneShotTime = atomic_getTimeCnt() + time;
}

// Duty設定
void TIMER_SetLcdDuty(uint8_t duty)
{
    lcd_pwmduty = duty;
}
