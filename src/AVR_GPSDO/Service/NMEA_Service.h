/*****************************************************
Title   :   NMEA Service
Author  :   kingyo
File    :   NMEA_Service.h
DATE    :   2020/07/05
******************************************************/
#ifndef __NMEA_SERVICE_H__
#define __NMEA_SERVICE_H__

#include <avr/io.h>

/*********************************
 * Prptotype
 *********************************/
void NMEA_Init(void);
uint8_t NMEA_GetNMEA(uint8_t stIndex, char *buf, uint8_t fillNullChar);
uint8_t NMEA_GetUTCDate(uint8_t stIndex, char *buf, uint8_t fillNullChar);
uint8_t NMEA_GetAramStatus(void);
void NMEA_SetUartThrough(uint8_t throughEn);

#endif //__MEAS_SERVICE_H__