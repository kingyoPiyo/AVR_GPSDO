/*****************************************************
Title   :   Encoder Driver
Author  :   kingyo
File    :   encoder.h
DATE    :   2020/10/18
******************************************************/
#ifndef __ENCODER_H__
#define __ENCODER_H__

/*********************************
 * ENUM
 *********************************/
typedef enum {
    ENUM_ENCODER_DIR_NONE = 0,
    ENUM_ENCODER_DIR_CW,
    ENUM_ENCODER_DIR_CCW,
    ENUM_ENCODER_DIR_NUM
} ENUM_ENCODER_DIR;

typedef enum {
    ENUM_ENCODER_PSWST_NONE = 0,
    ENUM_ENCODER_PSWST_PRESSED
} ENUM_ENCODER_PSWST;

/*********************************
 * Prptotype
 *********************************/
void ENCODER_Init(void);
ENUM_ENCODER_DIR ENCODER_GetDir(void);
ENUM_ENCODER_PSWST ENCODER_GetPushSWState(void);

#endif /* __ENCODER_H__ */
