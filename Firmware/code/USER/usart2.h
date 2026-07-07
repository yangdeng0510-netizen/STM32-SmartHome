#ifndef __USART2_H
#define __USART2_H

#include "stm32f4xx.h"
#include "stdio.h"
#include "string.h"

#define WIFI_RX_BUF_SIZE    1024

typedef struct
{
    char buf[WIFI_RX_BUF_SIZE];
    uint8_t flag;
    uint16_t len;
} WIFI_RECV;

extern WIFI_RECV WIFI_Rec;

void USART2_Init(uint32_t baud);
void USART2_SendByte(uint8_t data);
void USART2_SendStr(char *buf);
uint8_t USART2_RecvByte(void);
void WIFI_Rec_Clear(void);

#endif

