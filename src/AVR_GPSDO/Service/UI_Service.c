/*****************************************************
Title   :   UI Service
Author  :   kingyo
File    :   UI_Service.c
DATE    :   2020/07/05
******************************************************/
#include "UI_Service.h"
#include "mcu.h"
#include "lcd.h"
#include "led.h"
#include "timer.h"
#include "sound.h"
#include "encoder.h"
#include "FCNT_Service.h"
#include "NMEA_Service.h"
#include <stdio.h>


#define DEF_LCD_STRBUF_SIZE (20)
static const char* gBootText = "GPSDO Ver.1.00\n2020-10-18";

/*****************************
 * Enum
 *****************************/
typedef enum {
    ENUM_UI_MENU_FREQVIEW1 = 0,
    ENUM_UI_MENU_FREQVIEW2,
    ENUM_UI_MENU_CLKANDSATINFO,
    ENUM_UI_MENU_DACREGISTERSET,
    ENUM_UI_MENU_PPSSOUNDSET,
    ENUM_UI_MENU_UARTTHRSET,
    ENUM_UI_MENU_LCDBACKLGTSET,
    ENUM_UI_MENU_NUM
} ENUM_UI_MENU;

/*****************************
 * Typedef
 *****************************/
typedef void (*ui_menuFunc)(ENUM_ENCODER_DIR dir);

/*****************************
 * Global
 *****************************/
static uint8_t gPPSSoundEnable = 0;

/**********************************************************
 * 各種画面表示＆設定用関数群
 **********************************************************/
//周波数表示1(Gate = 1s, 100s)
static void ui_menu_FreqView1(ENUM_ENCODER_DIR dir)
{
    char tStr[DEF_LCD_STRBUF_SIZE];
    uint16_t tAveCnt;
    uint64_t tFreq = FCNT_GetFreq(ENUM_FCNT_FREQ_GATET_1, &tAveCnt);
    uint16_t tDacVal = FCNT_GetDacVal();
    
    sprintf(tStr,"%8luHz %4d ", (uint32_t)tFreq, tDacVal);
    lcd_puts(tStr);
    lcd_gotoxy(0, 1);
    tFreq = FCNT_GetFreq(ENUM_FCNT_FREQ_GATET_100, &tAveCnt);
    sprintf(tStr,"%10luHz %2d   ", (uint32_t)tFreq, tAveCnt);
    lcd_puts(tStr);
}

// 周波数表示2(Gate = 1000s)
static void ui_menu_FreqView2(ENUM_ENCODER_DIR dir)
{
    char tStr[DEF_LCD_STRBUF_SIZE];
    uint16_t tAveCnt;
    uint64_t tFreq = FCNT_GetFreq(ENUM_FCNT_FREQ_GATET_1, &tAveCnt);

    tFreq = FCNT_GetFreq(ENUM_FCNT_FREQ_GATET_1000, &tAveCnt);
    sprintf(tStr,"%08lu.%03luHz%03d  ", (uint32_t)(tFreq / 1000), (uint32_t)(tFreq % 1000), tAveCnt);
    lcd_puts(tStr);
    lcd_gotoxy(0, 1);
    NMEA_GetNMEA(0, tStr, 1);
    lcd_puts(tStr);
}

// UTC時刻、衛星情報表示
static void ui_menu_ClockAndSatelliteInfo(ENUM_ENCODER_DIR dir)
{
    char tStr[DEF_LCD_STRBUF_SIZE];

    NMEA_GetUTCDate(0, tStr, 1);
    lcd_puts(tStr);
    lcd_gotoxy(0, 1);
    NMEA_GetNMEA(0, tStr, 1);
    lcd_puts(tStr);
}

// DACレジスタ設定
static void ui_menu_DACRegisterSetting(ENUM_ENCODER_DIR dir)
{
    char tStr[DEF_LCD_STRBUF_SIZE];
    uint16_t tDacVal = FCNT_GetDacVal();

    if (dir == ENUM_ENCODER_DIR_CW)
    {
        if (tDacVal < 4096) tDacVal++;
    }
    else if (dir == ENUM_ENCODER_DIR_CCW)
    {
        if (tDacVal > 0) tDacVal--;
    }
    FCNT_SetDacVal(tDacVal);
    lcd_puts("DAC Register Val");
    lcd_gotoxy(0, 1);
    sprintf(tStr,"            %4d", tDacVal);
    lcd_puts(tStr);
}

// PPS音設定
static void ui_menu_PPSSoundSetting(ENUM_ENCODER_DIR dir)
{
    lcd_puts("PPS SOUND ON/OFF");
    lcd_gotoxy(0, 1);
    if (dir != ENUM_ENCODER_DIR_NONE)
    {
        gPPSSoundEnable = 1 - gPPSSoundEnable;
    }
    if (gPPSSoundEnable)
    {
        lcd_puts("              ON");
    }
    else
    {
        lcd_puts("             OFF");
    }
}

// UARTスルー設定
static void ui_menu_UARTThroughSetting(ENUM_ENCODER_DIR dir)
{
    static uint8_t gUART_pass = 0;

    lcd_puts("UART THRU ON/OFF");
    lcd_gotoxy(0, 1);
    if (dir != ENUM_ENCODER_DIR_NONE)
    {
        gUART_pass = 1 - gUART_pass;
        NMEA_SetUartThrough(gUART_pass);
    }
    if (gUART_pass)
    {
        lcd_puts("              ON");
    } 
    else
    {
        lcd_puts("             OFF");
    }
}

// LCDバックライト設定
static void ui_menu_LCDBacklightSetting(ENUM_ENCODER_DIR dir)
{
    char tStr[DEF_LCD_STRBUF_SIZE];
    static uint8_t lcd_duty = 2;

    lcd_puts("LCD Backlight   ");
    lcd_gotoxy(0, 1);
    if (dir == ENUM_ENCODER_DIR_CW)
    {
        if (lcd_duty < 5) lcd_duty++;
    }
    else if (dir == ENUM_ENCODER_DIR_CCW)
    {
        if (lcd_duty > 0) lcd_duty--;
    }
    sprintf(tStr,"               %1d", lcd_duty);
    lcd_puts(tStr);
    TIMER_SetLcdDuty(lcd_duty + 1);
}

// UIメニュー関数テーブル
static ui_menuFunc ui_menuFuncTbl[ENUM_UI_MENU_NUM] = {
    ui_menu_FreqView1,
    ui_menu_FreqView2,
    ui_menu_ClockAndSatelliteInfo,
    ui_menu_DACRegisterSetting,
    ui_menu_PPSSoundSetting,
    ui_menu_UARTThroughSetting,
    ui_menu_LCDBacklightSetting
};

/*****************************
 * PPS音再生
 *****************************/
void UI_PPSSound(void)
{
    if (gPPSSoundEnable)
    {
        SOUND_Start(47);
        TIMER_SetOneshotFunc(SOUND_Stop, 10);
    }
}

/*****************************
 * 起動メッセージ表示
 *****************************/
void UI_InitMsg(void)
{
    uint8_t i;
    uint32_t tBaseTime;

    // LED全点灯
    LED_SetState(ENUM_LED_SEL_LOCK, ENUM_LED_STATE_ON);
    LED_SetState(ENUM_LED_SEL_ALM, ENUM_LED_STATE_ON);

    // LCD初期化
    lcd_init(LCD_DISP_ON);
    lcd_puts(gBootText);
    tBaseTime = TIMER_GetNowTimeCnt();
    for (i = 0; i < 40; i++)
    {
        while (TIMER_GetElapsedTime(tBaseTime) < 50)
        {
            MCU_ResetWDT();
        }
        tBaseTime = TIMER_GetNowTimeCnt();

        // 起動サウンド
        if (i == 23) SOUND_Start(60);
        if (i == 24) SOUND_Start(44);
        if (i == 25) SOUND_Stop();
    }
    lcd_clrscr();

    // LED消灯
    LED_SetState(ENUM_LED_SEL_LOCK, ENUM_LED_STATE_OFF);
    LED_SetState(ENUM_LED_SEL_ALM, ENUM_LED_STATE_OFF);
}

/*****************************
 * サービス初期化
 *****************************/
void UI_Init(void)
{
    TIMER_SetLcdDuty(3);
}

/*****************************
 * UIメインルーチン
 *****************************/
void UI_Main(void)
{
    uint16_t tAveCnt;
    uint64_t tFreq;
    static uint8_t mode = 0;
    ENUM_ENCODER_DIR tEncDir;

	// ALM表示
	if (NMEA_GetAramStatus())
	{
        LED_SetState(ENUM_LED_SEL_ALM, ENUM_LED_STATE_ON);
	}
    else
    {
		LED_SetState(ENUM_LED_SEL_ALM, ENUM_LED_STATE_OFF);
	}

	// LOCK表示（10MHz±2mHz以内で点灯）
    tFreq = FCNT_GetFreq(ENUM_FCNT_FREQ_GATET_1000, &tAveCnt);
	if (tFreq >= 9999999998 && tFreq <= 10000000002)
	{
		LED_SetState(ENUM_LED_SEL_LOCK, ENUM_LED_STATE_ON);
	}
    else
    {
		LED_SetState(ENUM_LED_SEL_LOCK, ENUM_LED_STATE_OFF);
	}

    // ボタン状態取得
    if (ENCODER_GetPushSWState() == ENUM_ENCODER_PSWST_PRESSED)
    {
        if (++mode >= ENUM_UI_MENU_NUM) mode = 0;
    }

	// 各種メニュー表示
    lcd_gotoxy(0, 0);
    tEncDir = ENCODER_GetDir();
    ui_menuFuncTbl[mode](tEncDir);
}
