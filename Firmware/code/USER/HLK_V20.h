#ifndef __HLK_H
#define __HLK_H

#include "stm32f4xx.h"  

/* 1. 宏定义 (Defines) */


/* 2. 结构体/共用体声明 (Structures / Typedefs) */
// typedef struct { ... } MyStruct;

/* 3. 全局变量声明 (使用 extern) */
// extern uint8_t XXX_Status;
extern volatile uint8_t hlk_rx_buf[4];
extern volatile uint8_t hlk_rx_cnt;
extern volatile uint8_t hlk_rx_flag;
/* 4. 函数声明 (Function Prototypes) */
void HLK_Init(void);
uint8_t HLK_Cmd_Check(uint8_t d0,
                      uint8_t d1,
                      uint8_t d2,
                      uint8_t d3);

#endif   

