/*****************************************************
Title   :   Encoder Driver
Author  :   kingyo
File    :   encoder.c
DATE    :   2020/10/18
******************************************************/
#include "encoder.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

volatile static ENUM_ENCODER_DIR gEncDir = ENUM_ENCODER_DIR_NONE;

ISR (PCINT2_vect)
{
    static uint8_t old_enc_a;
    uint8_t tPIND = PIND;
    uint8_t enc_a = (tPIND & 0x04) >> 2;
    uint8_t enc_b = (tPIND & 0x08) >> 3;

    // ENC_Aの立ち上がり検出
    if (enc_a == 1 && old_enc_a == 0)
    {
        if (enc_b)
        {
            gEncDir = ENUM_ENCODER_DIR_CCW;
        }
        else
        {
            gEncDir = ENUM_ENCODER_DIR_CW;
        }
    }
    old_enc_a = enc_a;
}

// 初期化
void ENCODER_Init(void)
{
    // PCINT18,19,20(エンコーダ)割り込み設定
    PCICR   = 0b00000100;   // PCIE2
    PCMSK2  = 0b00000100;   // PCINT18割り込み許可
}

// 回転方向取得
ENUM_ENCODER_DIR ENCODER_GetDir(void)
{
    ENUM_ENCODER_DIR tEncDir;

    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
        tEncDir = gEncDir;
        gEncDir = ENUM_ENCODER_DIR_NONE;
    }

    return tEncDir;
}

// プッシュボタン状態取得
ENUM_ENCODER_PSWST ENCODER_GetPushSWState(void)
{
    ENUM_ENCODER_PSWST tSt = ENUM_ENCODER_PSWST_NONE;
    static uint8_t enc_psw_old;
    uint8_t enc_psw;

    enc_psw = (PIND & 0x10) >> 4;
	if (enc_psw == 0 && enc_psw_old == 1)
	{
		tSt = ENUM_ENCODER_PSWST_PRESSED;
	}
	enc_psw_old = enc_psw;

    return tSt;
}