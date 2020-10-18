/*****************************************************
Title   :   10MHz AVR GPSDO
Author  :   kingyo
File    :   main.c
DATE    :   2020/06/13
******************************************************/
#include "mcu.h"
#include "FCNT_Service.h"
#include "NMEA_Service.h"
#include "UI_Service.h"

int main(void)
{
    // MCU初期化
    MCU_Init();
    
    // サービス初期化
    FCNT_Init();
    NMEA_Init();
    UI_Init();
    
    // 全割り込み許可
    MCU_EnableGlobalInterrupt();
    
    // 起動メッセージ表示
    UI_InitMsg();

    // メインルーチン
    while (1) 
    {
        // 周波数制御処理
        FCNT_Main();

        // UI処理
        UI_Main();

        // ウォッチドッグタイマクリア
        MCU_ResetWDT();
    }
}
