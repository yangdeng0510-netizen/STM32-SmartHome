#ifndef _WIFI_H
#define _WIFI_H

#include "stm32f4xx.h"

#include "usart2.h"
#include "systick.h"
#include "stdio.h"
#include "string.h"
#include "MQTT.h"

#define WIFI_OK       0
#define WIFI_ERROR    1

extern WIFI_RECV WIFI_Rec;
void WIFI_Weather_Init(void);
char WIFI_SendAT(char* AT,char* respond);
void WIFI_TCP_Init(void);
void WIFI_ConnectServer(char*type,char *RemoteIP,char *RemotePort);
void WIFI_ExitTransmit(void);
uint8_t WIFI_SendCmd(char *cmd, char *ack, uint32_t wait_ms);
uint8_t WIFI_Init(void);
uint8_t WIFI_GetIP(void);
uint8_t WIFI_ConnectAP(char *ssid, char *password);
uint8_t WIFI_TCP_Test(void);
void WIFI_Check_Mode(void);
void WIFI_Echo_Off(void);
#endif
