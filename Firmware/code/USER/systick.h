#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f4xx.h"  

/* 1. 宏定义 (Defines) */
 #define  SYSTICK_CLOCK  168000000

/* 2. 结构体/共用体声明 (Structures / Typedefs) */
// typedef struct { ... } MyStruct;

/* 3. 全局变量声明 (使用 extern) */
// extern uint8_t XXX_Status;

/* 4. 函数声明 (Function Prototypes) */
void  delay_us(u32 xus);
void  delay_ms(u32 xms);
void  delay_s(u32 xs);

#endif /* __XXX_H */

